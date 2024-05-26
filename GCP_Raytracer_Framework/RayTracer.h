#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include "Ray.h"
#include "Sphere.h"
#include "Light.h"
#include "Cube.h"
#include "GameObject.h"

//#include <cmath>
#include <iostream>
#include <vector>

class RayTracer
{
public:
	RayTracer(int max);
	RayTracer(int maxD, int rNum, int sNum, float a, glm::vec3 ambi);
	~RayTracer();

	glm::vec3 TraceRay(Ray _ray, int _rDepth);
	glm::vec3 PBR(GameObject* _obj, interData _data, int _rDepth);
	glm::vec3 DebugShader(GameObject* _obj, interData _data);

	//Getters and Setters
	void addLight(Light* _light) { lights.push_back(_light); }
	void addObject(GameObject* _object) { objects.push_back(_object); }

private:
	//PBR Functions
	float DistributionGGX(glm::vec3 N, glm::vec3 H, float a);            //Normal Distribution Function
	float GeometrySmith(glm::vec3 N, glm::vec3 V, glm::vec3 L, float k); //Geometry Function
	float GeometrySchlickGGX(float NdotV, float k);                      //Sub function used in GeometrySmith
	glm::vec3 fresnelSchlick(float cosTheta, glm::vec3 F0);              //Fresnel Function (part of the calculation is done in F0 before being passing into this function!)
	float shadow(GameObject* _obj, interData _data, int _rDepth, Light* _light);

	std::vector<Light*> lights;
	std::vector<GameObject*> objects;
	glm::vec3 ambient;
	float alpha;       
	int shadowNum;
	int ReflectNum;
	int maxDepth;

};
#endif // !_TRACER	_H_



