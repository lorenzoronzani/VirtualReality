#include "Skybox.h"
#define FREEIMAGE_LIB
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "FreeImage.h"

#define GL_TEXTURE_MAX_ANISOTROPY_EXT        0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT    0x84FF



Skybox::Skybox()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    unsigned int cubeVboVertices;
    // Load cube data into a VBO:
    glGenBuffers(1, &cubeVboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    unsigned int cubeVboFaces;
    glGenBuffers(1, &cubeVboFaces);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVboFaces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeFaces), cubeFaces.data(), GL_STATIC_DRAW);
}

Skybox::~Skybox()
{
}

void LIB_API Skybox::load(const std::array<std::string,6>& file) {
	//Crea e fa il bind della texture
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

	// Set params:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Set filters:
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

    // Load sides:
    for (int curSide = 0; curSide < 6; curSide++)
    {
        // Load texture:
        FIBITMAP* fBitmap = FreeImage_Load(FreeImage_GetFileType(file[curSide].c_str(), 0), file[curSide].c_str());
        if (fBitmap == nullptr)
            std::cout << "[ERROR] loading file '" << file[curSide] << "'" << std::endl;
        int intFormat = GL_RGB;
        GLenum extFormat = GL_BGR;
        if (FreeImage_GetBPP(fBitmap) == 32)
        {
            intFormat = GL_RGBA;
            extFormat = GL_BGRA;
        }

        // Fix mirroring:
        FreeImage_FlipHorizontal(fBitmap);  // Correct mirroring from cube's inside
        FreeImage_FlipVertical(fBitmap);    // Correct JPG's upside-down

        // Send texture to OpenGL:
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + curSide, 0, intFormat, FreeImage_GetWidth(fBitmap), FreeImage_GetHeight(fBitmap), 0, extFormat, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(fBitmap));

        // Free resources:
        FreeImage_Unload(fBitmap);
    }
}

void LIB_API Skybox::render(glm::mat4 modelView, ShaderSettings& shader) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    glBindVertexArray(m_vao);


    glm::mat4 f = glm::mat4(1.0f);

    f = glm::translate(f, glm::vec3(0.0f, 0.0f, -999.0f));
    f = glm::scale(f, glm::vec3(100.0f, 100.0f, 100.0f));
    // Set model matrix as current OpenGL matrix:   
    shader.cubemapShader->setMatrix(shader.mvLocCubemap, f);
    glDrawElements(GL_TRIANGLES, cubeFaces.size(), GL_UNSIGNED_SHORT, nullptr);
}

void LIB_API Skybox::id(unsigned int id)
{
	m_id = id;
}

unsigned int LIB_API Skybox::id() const
{
	return m_id;
}
