#include "Texture.h"
#define FREEIMAGE_LIB
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "FreeImage.h"

#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF

void LIB_API Texture::load(const std::string& file,ShaderSettings &shader){
	if (file.find("[none]")==std::string::npos) {
		//Crea e fa il bind della texture
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);


		// Set circular coordinates:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Set min/mag filters:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

		//Filtro anisotropico
		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(file.c_str(), 0), file.c_str());

		int intFormat = GL_COMPRESSED_RGB;
		GLenum extFormat = GL_BGR;
		if (FreeImage_GetBPP(bitmap) == 32)
		{
			intFormat = GL_COMPRESSED_RGBA;
			extFormat = GL_BGRA;
		}

		//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap));
		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), 0, extFormat, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap));
		//Unloda texture
		FreeImage_Unload(bitmap);

		//Disattivo bind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void LIB_API Texture::render(glm::mat4 modelView,ShaderSettings &shader){

}

void LIB_API Texture::id(unsigned int id)
{
	m_id = id;
}

unsigned int LIB_API Texture::id() const
{
	return m_id;
}
