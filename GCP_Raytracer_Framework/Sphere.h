#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "GameObject.h"

struct interData;

class Sphere : public GameObject
{
public:
	Sphere(glm::vec3 _pos, float _r);
	~Sphere();

	interData Intersect(Ray _ray);

	//Getters
	float getRadius() { return m_Radius; }

private:
	glm::vec3 ClosestPoint(Ray, glm::vec3); //Findes closes point on the sphere to a ray
	bool IntersectCheck(glm::vec3);         //Checks if there is an intersection
	glm::vec3 IntersectPoint(Ray);          //Finds the intersection point on the sphere

	float m_Radius;    //Sphere radius
};

#endif