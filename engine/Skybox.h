#pragma once
#include "Object.h"
#include <iostream>
#include <string>
#include <cstring>
#include <array>

class LIB_API Skybox : public Object {
public:
	Skybox();
	~Skybox();

	void load(const std::array<std::string,6>& file);

	void render(glm::mat4 modelView, ShaderSettings& shader);

	void id(unsigned int id);
	unsigned int id() const;

private:
	unsigned int m_id;
	unsigned int m_vao;
	std::array<float,24> cubeVertices = // Vertex and tex. coords are the same
	{
	   -1.0f,  1.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
	   -1.0f,  1.0f, -1.0f,
	   -1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
	};
	std::array<unsigned short,36> cubeFaces =
	{
	   0, 1, 2,
	   0, 2, 3,
	   3, 2, 6,
	   3, 6, 7,
	   4, 0, 3,
	   4, 3, 7,
	   6, 5, 4,
	   7, 6, 4,
	   4, 5, 1,
	   4, 1, 0,
	   1, 5, 6,
	   1, 6, 2,
	};
};

