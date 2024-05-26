#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp> // This one lets us use matrix transformations
#include <iostream>
#include <limits> //To set float to max value

#include "Ray.h"

class Camera
{
public:
	Camera(glm::ivec2 _winSize, float _fov);
	~Camera();

	Ray castRay(glm::ivec2 _pos);

	//Getters and Setters
	glm::mat4 getWorldMat() { return worldMat; }

private:
	glm::vec2 MapCoordinate(glm::vec2 oldPixelCoord, glm::vec2 oldTL, glm::vec2 oldBR, glm::vec2 newTL, glm::vec2 newBR);

	glm::mat4 projectMat;
	glm::mat4 cameraMat;
	glm::mat4 worldMat;

	glm::vec2 winSize;
	float aspect;
	float fov;

};

#endif