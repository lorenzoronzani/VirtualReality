#pragma once
#include "Object.h"
#include <iostream>
#include <string>
#include <cstring>

class LIB_API Texture : public Object{
public:
	Texture() = default;
	~Texture() = default;

	void load(const std::string& file);

	void render(std::shared_ptr<Object> camera);

	void id(unsigned int id);
	unsigned int id() const;

private:
	unsigned int m_id;
};

