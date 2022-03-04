#include "Texture.h"
#define FREEIMAGE_LIB
#include <GL/freeglut.h>

#include "../dependencies/freeimage/include/FreeImage.h"

#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF

LIB_API Texture::Texture(){

}

LIB_API Texture::~Texture(){

}

void LIB_API Texture::load(const std::string& file){
	if (file.find("[none]")==std::string::npos) {
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		if (strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_texture_filter_anisotropic"))
		{
			int anisotropical_level;
			glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropical_level);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropical_level);
		}
		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(file.c_str(), 0), file.c_str());
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap));      
		FreeImage_Unload(bitmap);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void LIB_API Texture::settings(const TextureSettings& textureSettings){
	m_settings = textureSettings;
}

TextureSettings LIB_API Texture::settings() const{
	return m_settings;
}

void LIB_API Texture::render(std::shared_ptr<Object> camera){

}

void Texture::id(unsigned int id)
{
	m_id = id;
}

unsigned int Texture::id() const
{
	return m_id;
}
