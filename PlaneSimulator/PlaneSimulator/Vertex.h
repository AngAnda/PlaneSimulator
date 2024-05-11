#pragma once
#include <glm.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 color;
	glm::vec2 TexCoords;
	glm::vec3 Normal;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	int colorID = -1;
};