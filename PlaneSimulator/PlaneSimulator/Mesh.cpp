#include "Mesh.h"

Mesh::Mesh(std::string OBJfile, std::string PathFile)
{
	this->position = glm::vec3(0.f);
	this->rotation = glm::vec3(0.f);
	this->scale = glm::vec3(5.0f);
	std::pair <std::vector<Vertex>, std::vector<Material>> files = loadOBJ(OBJfile.c_str(), PathFile);
	this->vertices = files.first;
	this->materials = files.second;
	//initVAO();
	initMaterials();
	updateModelMatrix();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(2, &this->EBO);
}

void Mesh::initVertexData(Vertex* vertexArray, const unsigned& nrOfVertices, GLuint* indexArray, const unsigned& nrOfIndices)
{
	for (size_t i = 0; i < nrOfVertices; i++)
	{
		this->vertices.push_back(vertexArray[i]);
	}

	for (size_t i = 0; i < nrOfIndices; i++)
	{
		this->indices.push_back(indexArray[i]);
	}
}

void Mesh::updateModelMatrix()
{
	this->ModelMatrix = glm::mat4(1.f);
	this->ModelMatrix = glm::translate(this->ModelMatrix, this->position);
	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
	this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
}


void Mesh::initMaterials()
{
	for (int i = 0; i < materials.size(); i++)
	{
		for (int j = 0; j < vertices.size(); j++)
		{
			if (vertices[j].colorID == i)
			{
				vertices[j].ambient = materials[i].ambient;
				vertices[j].diffuse = materials[i].diffuse;
				vertices[j].specular = materials[i].specular;
			}
		}
	}
}


void Mesh::initVAO()
{
	//Create VAO
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	//Gen VBO and bind and send data
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// Gen EBO and bind and send data
	if (this->indices.size() > 0)
	{
		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW);
	}

	//Set VertexAtributesPoints and enable (input assembly)
	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::position));
	glEnableVertexAttribArray(0);
	//Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::color));
	glEnableVertexAttribArray(1);
	//Texcoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::texcoord));
	glEnableVertexAttribArray(2);
	//Normal
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::normal));
	glEnableVertexAttribArray(3);
	//Ambient
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::ambient));
	glEnableVertexAttribArray(4);
	//Diffuse
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::diffuse));
	glEnableVertexAttribArray(5);
	//Specular
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::specular));
	glEnableVertexAttribArray(6);

	//BIND VAO 0
	glBindVertexArray(0);

}
