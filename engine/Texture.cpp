#include "Texture.h"
#define FREEIMAGE_LIB
#include <GL/freeglut.h>

#include "FreeImage.h"

#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF

void LIB_API Texture::load(const std::string& file){
	if (file.find("[none]")==std::string::npos) {
		//Crea e fa il bind della texture
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Filtro anisotropico
		if (strstr((const char*)glGetString(GL_EXTENSIONS), "GL_EXT_texture_filter_anisotropic"))
		{
			int anisotropical_level;
			glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropical_level);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropical_level);
		}

		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(file.c_str(), 0), file.c_str());
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap));

		//Unloda texture
		FreeImage_Unload(bitmap);

		//Disattivo bind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void LIB_API Texture::render(glm::mat4 modelView){

}

void Texture::id(unsigned int id)
{
	m_id = id;
}

unsigned int Texture::id() const
{
	return m_id;
}
