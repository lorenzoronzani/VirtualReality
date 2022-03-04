#include "Material.h"

LIB_API Material::Material(){

}

LIB_API Material::~Material(){

}

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

void LIB_API Material::render(std::shared_ptr<Object> camera){

}
