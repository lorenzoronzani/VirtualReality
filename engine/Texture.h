#pragma once
#include "Object.h"
#include <iostream>
#include <string>
#include <cstring>

struct TextureSettings {

};

class LIB_API Texture : public Object{
public:
	Texture();
	~Texture();

	void load(const std::string& file);

	void settings(const TextureSettings& textureSettings);
	TextureSettings settings() const;

	void render(std::shared_ptr<Object> camera);

	void id(unsigned int id);

	unsigned int id() const;

private:
	TextureSettings m_settings;
	unsigned int m_id;
};

