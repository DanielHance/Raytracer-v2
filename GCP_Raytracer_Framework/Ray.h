#ifndef _RAY_H_
#define _RAY_H_

#include <GLM/glm.hpp>

class Ray
{
public:
	Ray();
	Ray(glm::vec3 _origin, glm::vec3 _dir);
	~Ray();

	glm::vec3 getOrigin() { return m_origin; }
	glm::vec3 getDir() { return m_dir; }
	void setOrigin(glm::vec3 _ori) { m_origin = _ori; }
	void setDir(glm::vec3 _dir) { m_dir = _dir; }

private:
	glm::vec3 m_origin;  //Ray Origen
	glm::vec3 m_dir;     //Ray Direction
};

#endif