#include "Cube.h"
#include <fstream>

Cube::Cube(glm::vec3 _pos, glm::vec3 _rotation, glm::vec3 _scale, std::string _path)
{
	//Set up object varables
	m_Pos = _pos;
	m_Rotation = _rotation;
	m_Scale = _scale;

	//Defult PBR Values
	m_albedo = glm::vec3(1, 0, 0);
	m_roughness = 0.5;
	m_metallic = 0.5;

	plane = false;
	if (_path == "Plane") plane = true;


	//Get Verts and face data from file
	std::string filename = "Shapes/" + _path + ".txt";
	std::ifstream file;

	file.open(filename);
	if (file.fail())
	{
		std::cout << "Failed to open shape file - " << filename << std::endl;
		return;
	}

	float x, y, z;
	char type;

	while (file >> type >> x >> y >> z)
	{
		if (type == 'v')
		{
			m_Verts.push_back(glm::vec3(x, y, z));
		}
		else if (type == 'f')
		{
			m_Faces.push_back(glm::vec3(x, y, z));
		}
	}

	//Create World Space Coords
	CreateCoords();
}


Cube::~Cube()
{
}


void Cube::CreateCoords() 
{
	glm::vec4 tranform;
	glm::mat4 scaleMat;
	glm::mat4 rotateMat;

	scaleMat = glm::scale(glm::mat4(1.0f), m_Scale);
	rotateMat = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
	rotateMat = glm::rotate(rotateMat, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
	rotateMat = glm::rotate(rotateMat, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

	for (int i = 0; i < m_Verts.size(); i++)
	{
		tranform = glm::vec4(m_Verts[i], 1.0f) * scaleMat;       //Scale
		tranform = tranform * rotateMat;                         //Rotate
		tranform = glm::vec4(glm::vec3(tranform) + m_Pos, 1.0f); //Translate
		m_Coords.push_back(glm::vec3(tranform));
		
		//Needs to be done this way as I need m_Coords to be a vec3 in world space, tranform is done manually to avoid it being added to the w component of a vec4,
		//this gives use a vec3 pos in world space :)
	} 
}


interData Cube::Intersect(Ray _ray)
{
	//Sets values of return data to a failed state so function can be exited early if no itersection is detected
	interData data;
	data.hit = false;
	data.point = glm::vec3(0, 0, 0);
	data.normal = glm::vec3(0, 0, 0);
	data.dir = _ray.getDir();
	float closesDis = 999999999;

	glm::vec3 dir(_ray.getDir());
	glm::vec3 ori(_ray.getOrigin());

	for (int i = 0; i < m_Faces.size(); i++)
	{
		glm::ivec3 face = m_Faces[i];

		//Points
		glm::vec3 p1 = m_Coords[face.x];
		glm::vec3 p2 = m_Coords[face.y];
		glm::vec3 p3 = m_Coords[face.z];

		//Center
		float cx = (p1.x + p2.x + p3.x) / 3.0f;
		float cy = (p1.y + p2.y + p3.y) / 3.0f;
		float cz = (p1.z + p2.z + p3.z) / 3.0f;
		glm::vec3 center(cx, cy, cz);

		//Edges
		glm::vec3 e1 = p2 - p1;
		glm::vec3 e2 = p3 - p2;
		glm::vec3 e3 = p1 - p3;

		//Normal
		glm::vec3 n = glm::cross(-e1, e2);
		n = glm::normalize(n);

		float d = -dot(n, p1);
		float t = -(dot(n, ori) + d) / dot(n, dir);

		//Check shape isn't behind ray
		if (t < 0.0f)
		{
			return data;
		}

		glm::vec3 p = ori + t * dir;

		glm::vec3 c1 = cross(e1, (p - p1));
		glm::vec3 c2 = cross(e2, (p - p2));
		glm::vec3 c3 = cross(e3, (p - p3));

		if(dot(n, c1) < 0 && dot(n, c2) < 0 && dot(n, c3) < 0)
		{ 

			//Local Depth Test
			if (glm::length(ori - p) < closesDis)
			{
				closesDis = glm::length(ori - p);

				//Fix Normal (for shading later)
				if (plane)
				{
					//Planes Normal
					if (dot(n, center) > 0.0) n = -n;
				}
				else
				{
					//Other shape Normal
					if (dot(n, m_Pos - center) > 0.0) n = -n;
				}

				//Return Values
				n = glm::normalize(n);
				data.hit = true;
				data.point = p;
				data.normal = n;
			}
		}
	}
	return data;
}

//ray - plane intersection formula from - https://www.youtube.com/watch?v=fIu_8b2n8ZM&ab_channel=JorgeRodriguez
//ray - triangle intersection formual from - https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html

//Barycentric Coordonates
/*
	glm::vec3 vA = -e3 - (((dot(e2, -e3)) / (dot(e2, e2))) * e2);
	glm::vec3 vB = -e1 - (((dot(e3, -e1)) / (dot(e3, e3))) * e3);
	glm::vec3 vC = -e2 - (((dot(e1, -e2)) / (dot(e1, e1))) * e1);
	float a = 1 - ((dot(vA, (p1 - p))));
	float b = 1 - ((dot(vB, (p2 - p))));
	float c = 1 - ((dot(vC, (p3 - p))));
	glm::vec3 bary(a, b, c);
*/