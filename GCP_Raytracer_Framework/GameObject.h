#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <GLM/glm.hpp>
#include <iostream>

#include "Ray.h"

//A structer holding data returned from Intersect()
struct interData
{
	bool hit;
	glm::vec3 point;
	glm::vec3 normal;
	glm::vec3 dir;
};


//Parent Object Class
class GameObject
{
public:
	
	virtual interData Intersect(Ray _ray) = 0; //Overloaded in cube and sphere classes

	//Getters and Setters
	glm::vec3 getPos() { return m_Pos; }
	glm::vec3 getAlbedo() { return m_albedo; }
	float getMetallic() { return m_metallic; }
	float getRoughness() { return m_roughness; }
	void setPBR(glm::vec3 _A, float _M, float _R)
	{
		m_albedo = _A;
		m_metallic = _M;
		m_roughness = _R;
	}

protected:

	glm::vec3 m_Pos;   //GameObject position
	glm::vec3 m_albedo;
	float m_metallic;
	float m_roughness;

};

#endif
