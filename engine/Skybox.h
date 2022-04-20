#pragma once
#include "Object.h"
#include <iostream>
#include <string>
#include <cstring>
#include <array>

class LIB_API Skybox : public Object {
public:
	Skybox() = default;
	~Skybox() = default;

	void load(const std::array<std::string,6>& file);

	void render(glm::mat4 modelView, ShaderSettings& shader);

	void id(unsigned int id);
	unsigned int id() const;

private:
	unsigned int m_id;
};

