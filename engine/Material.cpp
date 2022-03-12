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

void LIB_API Material::render(glm::mat4 modelView){
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, settings().roughness);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(settings().ambient));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(settings().diffuse));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(settings().specular));

    //Bind della texture
    if (texture()->name().find("[none]") == std::string::npos) {
        glBindTexture(GL_TEXTURE_2D, texture()->id());
    }else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
