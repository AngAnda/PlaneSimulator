//#include "Mesh.h"
//
//Mesh::Mesh(std::string OBJfile, std::string PathFile)
//{
//	this->position = glm::vec3(0.f);
//	this->rotation = glm::vec3(0.f);
//	this->scale = glm::vec3(5.0f);
//	std::pair <std::vector<Vertex>, std::vector<Material>> files = loadOBJ(OBJfile.c_str(), PathFile);
//	this->vertices = files.first;
//	this->materials = files.second;
//	initVAO();
//	initMaterials();
//	updateModelMatrix();
//}
//
//Mesh::~Mesh()
//{
//	glDeleteVertexArrays(1, &this->VAO);
//	glDeleteBuffers(1, &this->VBO);
//	glDeleteBuffers(2, &this->EBO);
//}
//
//void Mesh::initVertexData(Vertex* vertexArray, const unsigned& nrOfVertices, GLuint* indexArray, const unsigned& nrOfIndices)
//{
//	for (size_t i = 0; i < nrOfVertices; i++)
//	{
//		this->vertices.push_back(vertexArray[i]);
//	}
//
//	for (size_t i = 0; i < nrOfIndices; i++)
//	{
//		this->indices.push_back(indexArray[i]);
//	}
//}
//
//void Mesh::updateModelMatrix()
//{
//	this->ModelMatrix = glm::mat4(1.f);
//	this->ModelMatrix = glm::translate(this->ModelMatrix, this->position);
//	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
//	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
//	this->ModelMatrix = glm::rotate(this->ModelMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
//	this->ModelMatrix = glm::scale(this->ModelMatrix, this->scale);
//}
//
//
//void Mesh::initMaterials()
//{
//	for (int i = 0; i < materials.size(); i++)
//	{
//		for (int j = 0; j < vertices.size(); j++)
//		{
//			if (vertices[j].colorID == i)
//			{
//				vertices[j].ambient = materials[i].ambient;
//				vertices[j].diffuse = materials[i].diffuse;
//				vertices[j].specular = materials[i].specular;
//			}
//		}
//	}
//}
//
//
//void Mesh::initVAO()
//{
//	//Create VAO
//	glGenVertexArrays(1, &this->VAO);
//	glBindVertexArray(this->VAO);
//
//	//Gen VBO and bind and send data
//	glGenBuffers(1, &this->VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
//	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
//
//	// Gen EBO and bind and send data
//	if (this->indices.size() > 0)
//	{
//		glGenBuffers(1, &this->EBO);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_STATIC_DRAW);
//	}
//
//	//Set VertexAtributesPoints and enable (input assembly)
//	//Position
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::Position));
//	glEnableVertexAttribArray(0);
//	//Color
//	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::color));
//	glEnableVertexAttribArray(1);
//	*///Texcoord
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::TexCoords));
//	glEnableVertexAttribArray(2);
//	//Normal
//	/*glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::Normal));
//	glEnableVertexAttribArray(3);
//	*///Ambient
//	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::ambient));
//	//glEnableVertexAttribArray(4);
//	////Diffuse
//	//glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::diffuse));
//	//glEnableVertexAttribArray(5);
//	////Specular
//	//glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Vertex::specular));
//	//glEnableVertexAttribArray(6);
//
//	////BIND VAO 0
//	glBindVertexArray(0);
//
//}
//
//
//void Mesh::render(Shader* shader)
//{
//	shader->use();
//	updateModelMatrix();
//	shader->setMat4("model", ModelMatrix);
//	glBindVertexArray(this->VAO);
//	if (this->indices.empty())
//		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
//	else
//		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//
//	//Cleanup
//	glBindVertexArray(0);
//	glUseProgram(0);
//}
//
//
//void Mesh::setColor(int index, glm::vec3 rgb)
//{
//	int found = 0;
//	for (int i = 0; i < vertices.size(); i++)
//	{
//		if (this->vertices[i].colorID == index)
//		{
//			this->vertices[i].color = rgb;
//			found++;
//		}
//	}
//	std::cout << "found: " << found << '\n';
//}
//
//void Mesh::setPosition(glm::vec3 position)
//{
//	this->position = position;
//	updateModelMatrix();
//}
//
//void Mesh::setRotation(glm::vec3 rotation)
//{
//	this->rotation = rotation;
//	updateModelMatrix();
//}
//
//void Mesh::setScale(glm::vec3 scale)
//{
//	this->scale = scale;
//	updateModelMatrix();
//}
//
//void Mesh::setModel(glm::mat4 Model)
//{
//	ModelMatrix = Model;
//	updateModelMatrix();
//}
//
//glm::mat4 Mesh::getModel()
//{
//	return ModelMatrix;
//}
//
//glm::vec3 Mesh::getRotation()
//{
//	return rotation;
//}
//
//glm::vec3 Mesh::getPosition()
//{
//	return position;
//}
//
//std::vector<Material> Mesh::getMaterials()
//{
//	return materials;
//}
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh()
{
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // Load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "texture_height")
            number = std::to_string(heightNr++);

        glUniform1i(glGetUniformLocation(0, (name + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}