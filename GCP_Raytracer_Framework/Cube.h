#ifndef _CUBE_H_
#define _CUBE_H_

#include <vector>
#include <String>
#include <GLM/gtc/matrix_transform.hpp>

#include "GameObject.h"

struct interData;

class Cube : public GameObject
{
public:
	Cube(glm::vec3 _pos, glm::vec3 _rotation, glm::vec3 _scale, std::string _path);
	~Cube();

	interData Intersect(Ray _ray);

	//Getters
	std::vector<glm::vec3> getCoords() { return m_Coords; }
	std::vector<glm::ivec3> getFaces() { return m_Faces; }
	
private:

	void CreateCoords();   //Moves the verts into world space

	std::vector<glm::vec3> m_Verts;  //Verts in model space
	std::vector<glm::vec3> m_Coords; //Verts in world space
	std::vector<glm::ivec3> m_Faces; //Connects verts to make faces
	glm::vec3 m_Rotation;
	glm::vec3 m_Scale;
	bool plane;
};

#endif