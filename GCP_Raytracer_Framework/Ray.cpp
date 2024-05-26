#include "Ray.h"

Ray::Ray()
{
}


Ray::Ray(glm::vec3 _ori, glm::vec3 _dir)
{
	m_origin = _ori;
	m_dir = _dir;
}

Ray::~Ray()
{
}