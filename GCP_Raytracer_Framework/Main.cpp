#include "GCP_GFX_Framework.h"

#include <thread>
#include <mutex>
#include <vector>

#include "Camera.h"
#include "Ray.h"
#include "RayTracer.h"
#include "Sphere.h"
#include "Light.h"
#include "Cube.h"

void colourPixel(glm::ivec2 winSize, Camera* camera, Ray ray, RayTracer* tracer, GCP_Framework* _myFramework, int rDepth, int loops, int offset, int max); //Forward Declaration 

int main(int argc, char* argv[])
{
	// Set window size
	glm::ivec2 winSize(640, 480);

	// This will handle rendering to screen
	GCP_Framework _myFramework;

	// Initialises SDL and OpenGL and sets up a framebuffer
	if (!_myFramework.Init(winSize))
	{
		return -1;
	}

	std::cout << "\n\n\nLoading Objects...." << std::endl;

	//Key Varables
	glm::vec3 pixelCol;
	bool running = true;

	//Tracer Values
	int rDepth = 1;
	int shadowNum = 400;
	int reflectNum = 250;
	float alpha = 0.025f;
	glm::vec3 ambient(0.03f);

	//Threads
	int threadNum = 16;

	//Key Components
	Camera camera(winSize, 30.0); //fov
	Ray ray;
	RayTracer tracer(rDepth, reflectNum, shadowNum, alpha, ambient);

	//--------------------------------------Create all objects

	/*
	Formats:
		Create Spheres (pos, radius)
		Creat Cube (pos, rotation, scale, shape)
		Create Light (pos, brightness, colour)
		.setPBR(colour, metalic, roughtness)
	*/
	
	Sphere sphere1(glm::vec3(-5, 2, -15), 2);
	sphere1.setPBR(glm::vec3(0, 1, 0), 1, 0.01);
	Sphere sphere2(glm::vec3(5, 2, -15), 2);
	sphere2.setPBR(glm::vec3(0, 0, 1), 1, 0.9);
	Sphere sphere3(glm::vec3(0, 0.5, -15), 2);
	sphere3.setPBR(glm::vec3(1, 1, 0), 0, 0.9);

	Cube cube1(glm::vec3(0, 6, -15), glm::vec3(20, 27, 0), glm::vec3(3, 3, 3), std::string("Cube"));
	cube1.setPBR(glm::vec3(1, 0, 0), 1, 0.5);

	Cube floor(glm::vec3(0, -2, 0), glm::vec3(90, 0, 0), glm::vec3(60, 60, 60), std::string("Plane"));
	floor.setPBR(glm::vec3(1, 0, 0), 0.5, 1);

	Cube wall(glm::vec3(0, 28, -30), glm::vec3(0, 0, 0), glm::vec3(60, 60, 60), std::string("Plane"));
	wall.setPBR(glm::vec3(1, 1, 1), 1, 0.9);

	Light light2(glm::vec3(-5, 3, 0), 1.0f, glm::vec3(1, 1, 1), 10); 



	//---------------------------Add Objects To Shader
	
	tracer.addObject(&sphere1);
	tracer.addObject(&sphere2);
	tracer.addObject(&sphere3);
	tracer.addObject(&cube1);

	tracer.addObject(&floor);
	tracer.addObject(&wall);

	tracer.addLight(&light2);


	auto start = std::chrono::system_clock::now();
	std::cout << "Scene Rendereing...." << std::endl;

	while (running)
	{
		std::vector<std::thread> threads;
		size_t max = winSize.x * winSize.y;
		_myFramework.SetAllPixels(glm::vec3(0.03));

		//Create Thread
		for (int i = 0; i < threadNum; i++)
		{
			int loops = max / threadNum;
			int offset = loops * i;

			threads.push_back(std::thread(colourPixel, winSize, &camera, ray, &tracer, &_myFramework, rDepth, loops, offset, max));
		}

		//Wait for treads to finish
		for (int i = 0; i < threadNum; i++)
		{
			threads[i].join();
		}

		std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;

		std::cout << "Finished!" << std::endl;
		std::cout << "Time taken = " << dur.count() << std::endl;

		// Pushes the framebuffer to OpenGL and renders to screen
		// Also contains an event loop that keeps the window going until it's closed
		_myFramework.ShowAndHold();
	}
	return 0;
}

void colourPixel(glm::ivec2 winSize, Camera* camera, Ray ray, RayTracer* tracer, GCP_Framework* _myFramework,int rDepth, int loops, int offset, int max)
{
	for (int p = offset; p < loops + offset; p++)
	{
		if (p > max) break;

		int w = p / winSize.y;
		int h = p % winSize.y;
		glm::ivec2 pixelPos(w, h);

		ray = camera->castRay(pixelPos);
		glm::vec3 pixelCol = tracer->TraceRay(ray, rDepth);
		_myFramework->DrawPixel(pixelPos, pixelCol);
	}
}

			