#include "Light.h"

Light::Light(glm::vec3 _pos, float _r, glm::vec3 _col, float _b)
{
	m_Pos = _pos;
	m_Radius = _r;
	m_Col = _col;
	m_Brightness = _b;
}

Light::~Light()
{
}


glm::vec3 Light::getSample()
{
	std::random_device seed;  // Creates the seed 'seed' for the random engin
	std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count()* seed()* clock()); // Creates 'gen' a mersenne_twister_engine seeded with seed()
	std::uniform_real_distribution<> dis(-1.0f, 1.0f);


	glm::vec3 _pos(dis(gen), dis(gen), dis(gen));
	_pos* m_Radius;

	return _pos + m_Pos;
}
