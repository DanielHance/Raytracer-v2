#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <GLM/glm.hpp>
#include <random>
#include <chrono>


class Light
{
public:

	Light(glm::vec3 _pos, float _r, glm::vec3 _col, float _b);
	~Light();

	//Getters and Setters
	glm::vec3 getPos() { return m_Pos; }
	glm::vec3 getSample();
	glm::vec3 getCol() { return m_Col; }
	float getBrightness() { return m_Brightness; }

private:
	glm::vec3 m_Pos;
	float m_Radius;
	glm::vec3 m_Col;
	float m_Brightness;
};

#endif 
