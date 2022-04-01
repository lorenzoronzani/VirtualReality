#include "Material.h"
#include <glm/gtc/type_ptr.hpp>
#include <GL/freeglut.h>

void LIB_API Material::settings(const MaterialSettings& materialSettings){
	m_settings = materialSettings;
}

MaterialSettings LIB_API Material::settings() const{
	return m_settings;
}

void LIB_API Material::texture(std::shared_ptr<Texture> texture){
	m_texture = texture;
}

std::shared_ptr<Texture> Material::texture() const{
	return m_texture;
}

void LIB_API Material::render(glm::mat4 modelView, ShaderSettings &shader){
    // Set initial material and light params:
    shader.m_shader->setVec3(shader.matEmissionLoc, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.m_shader->setVec3(shader.matAmbientLoc, settings().ambient);
    shader.m_shader->setVec3(shader.matDiffuseLoc, settings().diffuse);
    shader.m_shader->setVec3(shader.matSpecularLoc, settings().specular);
    shader.m_shader->setFloat(shader.matShininessLoc, 128.0f);
    //Bind della texture
    if (texture()->name().find("[none]") == std::string::npos) {
        glBindTexture(GL_TEXTURE_2D, texture()->id());
    }else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
