#pragma once
#include "Object.h"
class ShaderSetup {
public:
	ShaderSetup(ShaderSettings& shader);
	void setupShader();
	void setupFboShader();
	int* viewport();
	unsigned int vao();
	unsigned int sizeXFbo();
	unsigned int sizeYFbo();
private:
	ShaderSettings& m_shader;
	int prevViewPort[4];
	unsigned int m_vao;
	unsigned int m_sizeXFbo;
	unsigned int m_sizeYFbo;
};