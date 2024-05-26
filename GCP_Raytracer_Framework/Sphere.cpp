#include "Sphere.h"


Sphere::Sphere(glm::vec3 _pos, float _r)
{
	m_Pos = _pos;
	m_Radius = _r;

	//Defult PBR Values
	m_albedo = glm::vec3 (1, 0, 0); //Red
	m_roughness = 0.5f;
	m_metallic = 0.5f;
}


Sphere::~Sphere()
{
}


interData Sphere::Intersect(Ray _ray)
{
	interData data;
	data.hit = false;
	data.point = glm::vec3(0, 0, 0);
	data.normal = glm::vec3(0, 0, 0);
	data.dir = _ray.getDir();

	//Checks that the sphere isn't behind the camera
	if (glm::dot(m_Pos - _ray.getOrigin(), _ray.getDir()) < 0)
	{
		//std::cout << "Behind" << std::endl;
		return data;
	}
	

	glm::vec3 linePoint = ClosestPoint(_ray, m_Pos); 

	//Check for sphere intersection
	if (IntersectCheck(linePoint) == false) 
	{
		return data;
	}
	
	//std::cout << "HIT" << std::endl;

	glm::vec3 _point = IntersectPoint(_ray);

	data.hit = true; 
	data.point = _point;
	data.normal = normalize(_point - m_Pos); 

	return data;
	
}


bool Sphere::IntersectCheck(glm::vec3 _point) //Takes a vec3 point and check if it is inside/intersects the sphere
{
	glm::vec3 distance = m_Pos - _point; 
	if (length(distance) <= m_Radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}



glm::vec3 Sphere::ClosestPoint(Ray _ray, glm::vec3 _point) //Takes a ray and a vec3 point and returns the closes point on the ray to _point
{
	glm::vec3 n = glm::normalize(_ray.getDir()); 
	glm::vec3 a = _ray.getOrigin(); 
	glm::vec3 x = a + (dot((_point - a), n)) * n; 
	return x;
}



glm::vec3 Sphere::IntersectPoint(Ray _ray) //Takes a ray and returns the closest intersecting point with the sphere
{
	glm::vec3 n = glm::normalize(_ray.getDir()); 
	glm::vec3 a = _ray.getOrigin(); 
	float d = glm::length(m_Pos - a - (glm::dot((m_Pos - a), n)) * n); 
	float x = sqrt(pow(m_Radius, 2) - pow(d, 2)); 
	glm::vec3 interPoint = a + ((glm::dot((m_Pos - a), n)) - x) * n; 
	return interPoint;
}