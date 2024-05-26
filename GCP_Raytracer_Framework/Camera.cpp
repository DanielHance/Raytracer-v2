#include "Camera.h"


Camera::Camera(glm::ivec2 _winSize, float _fov)
{
	winSize = _winSize;
	fov = _fov;
	aspect = winSize.x / winSize.y;


	projectMat = glm::perspective(glm::radians(_fov), aspect, 0.5f, 100.0f);
	cameraMat = glm::mat4(1.0f);

	worldMat = glm::inverse(cameraMat) * glm::inverse(projectMat); //Need to go backwards though spaces to get to world space (clip->view->world)

}
Camera::~Camera()
{
}


Ray Camera::castRay(glm::ivec2 _pos)
{
	glm::vec2 newPos(MapCoordinate(_pos, glm::vec2(0, 0), winSize, glm::vec2(-1, 1), glm::vec2(1, -1))); //Normalizes all coords into a 1 by -1 cube

	glm::vec4 nearPoint(newPos.x, newPos.y, -1, 1); //Creates the near plain point
	glm::vec4 farPoint(newPos.x, newPos.y, 1, 1);  //Creates the far palne point

	nearPoint = worldMat * nearPoint;
	farPoint = worldMat * nearPoint;

	glm::vec3 newNearPoint(nearPoint / nearPoint.w);
	glm::vec3 newFarPoint(farPoint / farPoint.w);

	Ray ray(newNearPoint, normalize(newFarPoint - newNearPoint));

	//Ray ray(newNearPoint, glm::vec3(0, 0, -1));

	return ray;
}


glm::vec2 Camera::MapCoordinate(glm::vec2 oldPixelCoords, glm::vec2 oldTL, glm::vec2 oldBR, glm::vec2 newTL, glm::vec2 newBR) //Maps the coordinates
{
	//glm::vec2 newPixelCoords = (oldPixelCoords - oldTL) * (newBR - newTL) / (oldBR - oldTL) + newTL;
	glm::vec2 newPixelCoords = ((oldPixelCoords - oldTL) / (oldBR - oldTL) ) * (newBR - newTL)  + newTL;

	return newPixelCoords;
}

//castRay function is broken