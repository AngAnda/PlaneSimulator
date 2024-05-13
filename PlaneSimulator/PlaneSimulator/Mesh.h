#pragma once
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

#include <string>
#include <vector>

using namespace std;

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLint diffuseTex;
	GLint speculatTex;
};

class Mesh
{
private:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Material> materials;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	glm::mat4 ModelMatrix;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	void initVertexData(Vertex* vertexArray, const unsigned& nrOfVertices, GLuint* indexArray, const unsigned& nrOfIndices);
	void updateModelMatrix();
	void initMaterials();

public:
	Mesh(std::string OBJfile, std::string PathFile);
	~Mesh();
	void update();
	void initVAO();
	void render(Shader* shader);
	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setModel(glm::mat4 Model);
	void setScale(glm::vec3 scale);
	void setColor(int index, glm::vec3 rgb);
	glm::mat4 getModel();
	glm::vec3 getRotation();
	glm::vec3 getPosition();
	std::vector <Material> getMaterials();
};

