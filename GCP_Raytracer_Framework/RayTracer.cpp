#include "RayTracer.h"

#include <random>
#include <chrono>
#include <iostream>
#include <GLM/glm.hpp>

RayTracer::RayTracer(int max)
{
	ambient = glm::vec3(0.03f); //Ambient Col
	alpha = 0.025f;            //Ambient Brightness 0.025
	ReflectNum = 100;
	shadowNum = 100;
	maxDepth = max;

}

RayTracer::RayTracer(int max, int rNum, int sNum, float a, glm::vec3 ambi)
{
	ambient = ambi;
	alpha = a;   
	maxDepth = max;
	ReflectNum = rNum;
	shadowNum = sNum;
}

RayTracer::~RayTracer()
{
}


glm::vec3 RayTracer::TraceRay(Ray _ray, int _rDepth)
{
	glm::vec3 pixelColour = ambient;
	int object = -1;
	float closesDis = 99999999;
	interData data;

	for (int i = 0; i < objects.size(); i++)
	{
		data = objects[i]->Intersect(_ray);

		if (data.hit && (glm::length(_ray.getOrigin() - data.point) < closesDis))
		{
			object = i;
			closesDis = glm::length(_ray.getOrigin() - data.point);
			
			//std::cout << "Normal = " << data.normal.x << ", " << data.normal.y << ", " << data.normal.z << std::endl;
		}
	}

	if (object != -1)
	{
		pixelColour = PBR(objects[object], objects[object]->Intersect(_ray), _rDepth);
		//pixelColour = DebugShader(objects[object], objects[object]->Intersect(_ray));
		
	}
	
	return pixelColour;
}


glm::vec3 RayTracer::PBR(GameObject* _obj, interData _data, int _rDepth)
{
	float PI = 3.14159265359;

	//Changing the names of ._data
	glm::vec3 worldPos = _data.point;
	glm::vec3 normal = _data.normal;
	glm::vec3 rayDir = _data.dir;

	//Normalize ._data vec3s
	glm::vec3 N = normalize(normal);     //World space normal 
	glm::vec3 V = normalize(-worldPos);  //World space pos

	//std::cout << N.x << "< " << N.y << ", " << N.z << std::endl;

	//Get Object Data
	glm::vec3 albedo = _obj->getAlbedo();
	float metallic = _obj->getMetallic();
	float roughness = _obj->getRoughness();

	glm::vec3 F0 = glm::vec3(0.04);
	F0 = glm::mix(F0, albedo, metallic);

	//Reflectance equation - Add later
	glm::vec3 Lo = glm::vec3(0.0);
	float maxShadow = 1.0f;

	//Light calculation per light
	for (int i = 0; i < lights.size(); i++)
	{
		glm::vec3 lightPos = lights[i]->getPos();
		glm::vec3 lightCol = lights[i]->getCol();
		float brightness = lights[i]->getBrightness() * 50;
		glm::vec3 L = normalize(lightPos - worldPos);   //Light Direction
		glm::vec3 H = normalize(V + L);                 //Half vector between V and L
		float distance = length(lightPos - worldPos);
		float attenuation = 1.0 / (distance * distance);
		attenuation *= brightness;
		glm::vec3 radiance = lightCol * attenuation;

		//cook - torrance brdf
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		glm::vec3 F = fresnelSchlick(glm::max(dot(H, V), 0.0f), F0);

		glm::vec3 kS = F;
		glm::vec3 kD = glm::vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		glm::vec3 numerator = NDF * G * F;
		float denominator = 4.0 * glm::max(dot(N, V), 0.0f) * glm::max(dot(N, L), 0.0f) + 0.0001;
		glm::vec3 specular = numerator / denominator;

		float NdotL = glm::max(dot(N, L), 0.0f);

		float shadowP = shadow(_obj, _data, _rDepth, lights[i]);

		Lo += ((kD * albedo / PI + specular) * radiance * NdotL) * shadowP;

		if (maxShadow > shadowP) maxShadow = shadowP;
		
	}

	//Refection
	if (_rDepth > 0)
	{
		_rDepth--;

		//Setup
		glm::vec3 reflectDir = glm::normalize(rayDir - 2 * (dot(rayDir, N)) * N);
		glm::vec3 reflectCol(0.0f);
		float noiseAmp = 1.0f;

		//Diffuse Reflection
		std::random_device seed;  // Creates the seed 'seed' for the random engin
		std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count() * seed() * clock()); // Creates 'gen' a mersenne_twister_engine seeded with seed()
		std::uniform_real_distribution<> dis((1.0f - roughness) * noiseAmp, (1.0f + roughness) * noiseAmp); //Creates a unifor real distribution dis()
		Ray difReflectRay;

		for (int i = 0; i < ReflectNum; i++)
		{
			glm::vec3 noise(dis(gen), dis(gen), dis(gen));
			//noise = glm::vec3(1.0f, 1.0f, 1.0f);
			glm::vec3 difReflectDir = glm::normalize(reflectDir * noise);

			difReflectRay.setOrigin(worldPos + (reflectDir * 0.0001f));
			difReflectRay.setDir(difReflectDir);

			reflectCol += TraceRay(difReflectRay, _rDepth);
		}

		reflectCol = (reflectCol / float(ReflectNum)) * metallic;
		Lo += reflectCol * maxShadow;
	}


	//Add ambient light
	glm::vec3 _ambient = ambient * albedo * alpha;
	glm::vec3 colour = _ambient + Lo;

	//Gamma Correct
	colour = colour / (colour + glm::vec3(1.0f));
	colour = pow(colour, glm::vec3(1.0f / 2.2f));

	return colour;
}


float RayTracer::shadow(GameObject* _obj, interData _data, int _rDepth, Light* _light)
{
	int hits = 0;
	for (int i = 0; i < shadowNum; i++)
	{
		glm::vec3 lightPos;
		if (_rDepth != maxDepth) 
		{
			lightPos = _light->getPos();
		}
		else
		{
			lightPos = _light->getSample();
		}
		glm::vec3 worldPos = _data.point;
		glm::vec3 L = normalize(lightPos - worldPos);

		Ray shadowRay(worldPos, L);
		for (int o = 0; o < objects.size(); o++)
		{
			if (objects[o] != _obj)
			{
				interData data = objects[o]->Intersect(shadowRay);

				if (data.hit)
				{
					//std::cout << "HIT" << std::endl;
					if (glm::length(worldPos - data.point) < glm::length(worldPos - lightPos)) //Checks object isn't behind light
					{
						hits++;
					}
				}
			}
		}
		if (_rDepth != maxDepth) 
		{
			if (hits != 0)
			{
				return 0;
			}
			else
			{
				return 2;
			}
		}
	}

	return (float(shadowNum - hits)) / shadowNum;
}








float RayTracer::DistributionGGX(glm::vec3 N, glm::vec3 H, float a)
{
	float PI = 3.14159265359;
	float a2 = a * a;
	float NdotH = glm::max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}



float RayTracer::GeometrySmith(glm::vec3 N, glm::vec3 V, glm::vec3 L, float k)
{
	float NdotV = glm::max(dot(N, V), 0.0f);
	float NdotL = glm::max(dot(N, L), 0.0f);
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}


float RayTracer::GeometrySchlickGGX(float NdotV, float k)
{
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}


glm::vec3 RayTracer::fresnelSchlick(float cosTheta, glm::vec3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}



glm::vec3 RayTracer::DebugShader(GameObject* _obj, interData _data)
{
	glm::vec3 w = glm::normalize(lights[0]->getPos() - _data.point);// _obj->getPos();
	glm::vec3 n = _data.normal;
	glm::vec3 l = lights[0]->getCol();
	glm::vec3 k = _obj->getAlbedo();
	glm::vec3 col = dot(w, n)* l* k;

	//std::cout << n.x << ", " << n.y << ", " << n.z << std::endl;

	return col;
}

