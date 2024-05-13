#pragma once
#include <vector>
#include "Vertex.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLint diffuseTex;
	GLint speculatTex;
};



static std::vector<Material> loadMTL(const char* file_name)
{
	int pos = -1;
	std::vector<Material> materials;
	std::stringstream ss;
	std::ifstream fin(file_name);
	if (!fin.is_open())
	{
		std::cout << "Failed to load " << file_name << '\n';
		return std::vector<Material>();
	}

	std::string line;
	std::string prefix;
	GLint value;
	while (std::getline(fin, line))
	{
		//get prefix
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "newmtl") //vertex position
		{
			materials.push_back(Material());
			pos++;
		}
		else
			if (prefix == "Ka") //texture coords
			{
				glm::vec3 ambient;
				ss >> ambient.x >> ambient.y >> ambient.z;
				materials[pos].ambient = ambient;
			}
			else
				if (prefix == "Kd") //vertex normals
				{
					glm::vec3 diffuse;
					ss >> diffuse.x >> diffuse.y >> diffuse.z;
					materials[pos].diffuse = diffuse;
				}
				else
					if (prefix == "Ks") //vertex normals
					{
						glm::vec3 specular;
						ss >> specular.x >> specular.y >> specular.z;
						materials[pos].specular = specular;
					}
	}
	//debug
	std::cout << "MTL file " << file_name << " loaded successfully with " << pos + 1 << " materials!\n";
	return materials;
}

static std::pair<std::vector<Vertex>, std::vector<Material>> loadOBJ(const char* OBJfile_name, const std::string PathFile)
{

	//vertex vectors
	std::vector<glm::fvec3> vertex_position;
	std::vector<glm::fvec3> vertex_normal;
	std::vector<glm::fvec2> vertex_texcoord;

	//face vectors
	std::vector<GLint> vertex_position_indices;
	std::vector<GLint> vertex_texcoord_indices;
	std::vector<GLint> vertex_normal_indices;
	std::vector<GLint> color_indices;

	//Vertex array
	std::vector<Vertex> vertices;

	std::stringstream ss;
	std::ifstream fin(OBJfile_name);
	if (!fin.is_open())
	{
		std::cout << "Failed to load " << OBJfile_name << '\n';
		return std::make_pair(std::vector<Vertex>(), std::vector <Material>());
	}

	std::string line;
	std::string materialName;
	std::string prefix;
	glm::vec3 temp_vec3;
	glm::vec2 temp_vec2;
	GLint temp_glint;
	GLint matNumber = -1;
	while (std::getline(fin, line))
	{
		//get prefix
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "mtllib") //vertex position
		{
			ss >> materialName;
		}
		else
			if (prefix == "v") //vertex position
			{
				ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
				vertex_position.push_back(temp_vec3);
			}
			else
				if (prefix == "vt") //texture coords
				{
					ss >> temp_vec2.x >> temp_vec2.y;
					vertex_texcoord.push_back(temp_vec2);
				}
				else
					if (prefix == "vn") //vertex normals
					{
						ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
						vertex_normal.push_back(temp_vec3);
					}
					else
						if (prefix == "f")
						{
							int counter = 0;
							while (ss >> temp_glint)
							{
								//indices into correct arrays
								if (counter == 0)
								{
									vertex_position_indices.push_back(temp_glint);
									color_indices.push_back(matNumber);
								}
								else if (counter == 1)
									vertex_texcoord_indices.push_back(temp_glint);
								else if (counter == 2)
									vertex_normal_indices.push_back(temp_glint);

								//characters
								if (ss.peek() == '/')
								{
									counter++;
									ss.ignore(1, '/');
								}
								else if (ss.peek() == ' ')
								{
									counter++;
									ss.ignore(1, ' ');
								}

								//reset the counter
								if (counter > 2)
								{
									counter = 0;
								}
							}
						}
						else if (prefix == "usemtl")
						{
							matNumber++;
						}

	}

	//Load in all indices
	vertices.resize(vertex_position_indices.size(), Vertex());
	for (size_t i = 0; i < vertices.size(); i++)
	{
		vertices[i].Position = vertex_position[vertex_position_indices[i] - 1];
		vertices[i].TexCoords = vertex_texcoord[vertex_texcoord_indices[i] - 1];
		vertices[i].Normal = -0.5f * vertex_normal[vertex_normal_indices[i] - 1];
		vertices[i].colorID = color_indices[i];
		vertices[i].color = glm::vec3(1.0f, 0.0f, 1.0f);
	}

	//debug
	std::cout << "OBJ file " << OBJfile_name << " loaded successfully with " << vertices.size() << "vertices" << "!\n";
	std::vector <Material> materials;
	materialName = PathFile + materialName;
	std::ifstream mat(materialName);
	if (!mat.is_open())
	{
		mat.close();
		materialName = "AA/" + materialName;
		std::ifstream mat2(materialName);
		if (!mat2.is_open())
		{
			std::cout << materialName << " not found!";
		}
		else
		{
			mat.close();
			materials = loadMTL(materialName.c_str());
		}
	}
	else
	{
		mat.close();
		materials = loadMTL(materialName.c_str());
	}
	return std::make_pair(vertices, materials);
}
