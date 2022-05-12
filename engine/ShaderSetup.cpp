#include "ShaderSetup.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

ShaderSetup::ShaderSetup(ShaderSettings& shader,bool isVirtual):m_shader{shader},m_virtual{isVirtual}
{

}

//Inizializza ShaderSettings con le posizioni iniziali
void ShaderSetup::setupShader()
{
    m_shader.m_shader->render();
    m_shader.m_shader->bind(0, "in_Position");
    m_shader.m_shader->bind(1, "in_Normal");
    m_shader.m_shader->bind(2, "aTexCoord");

    // Get shader variable locations:
    m_shader.modelview = m_shader.m_shader->getParamLocation("modelview");
    m_shader.projection = m_shader.m_shader->getParamLocation("projection");
    m_shader.normalMatLoc = m_shader.m_shader->getParamLocation("normalMatrix");

    m_shader.matEmissionLoc = m_shader.m_shader->getParamLocation("matEmission");
    m_shader.matAmbientLoc = m_shader.m_shader->getParamLocation("matAmbient");
    m_shader.matDiffuseLoc = m_shader.m_shader->getParamLocation("matDiffuse");
    m_shader.matSpecularLoc = m_shader.m_shader->getParamLocation("matSpecular");
    m_shader.matShininessLoc = m_shader.m_shader->getParamLocation("matShininess");
    m_shader.num_lights = m_shader.m_shader->getParamLocation("numLights");
    m_shader.num_lights_spot = m_shader.m_shader->getParamLocation("numLightsSpot");

    for (int i = 0; i < 16; i++) {
        m_shader.lightSettings[i].lightPositionLoc = m_shader.m_shader->getParamLocation(std::string("lights[" + std::to_string(i) + "].lightPosition").c_str());
        m_shader.lightSettings[i].lightAmbientLoc = m_shader.m_shader->getParamLocation(std::string("lights[" + std::to_string(i) + "].lightAmbient").c_str());
        m_shader.lightSettings[i].lightDiffuseLoc = m_shader.m_shader->getParamLocation(std::string("lights[" + std::to_string(i) + "].lightDiffuse").c_str());
        m_shader.lightSettings[i].lightSpecularLoc = m_shader.m_shader->getParamLocation(std::string("lights[" + std::to_string(i) + "].lightSpecular").c_str());
    }
    for (int i = 0; i < 16; i++) {
        m_shader.lightSpotSettings[i].lightPositionLoc = m_shader.m_shader->getParamLocation(std::string("lightsSpot[" + std::to_string(i) + "].lightPosition").c_str());
        m_shader.lightSpotSettings[i].lightAmbientLoc = m_shader.m_shader->getParamLocation(std::string("lightsSpot[" + std::to_string(i) + "].lightAmbient").c_str());
        m_shader.lightSpotSettings[i].lightDiffuseLoc = m_shader.m_shader->getParamLocation(std::string("lightsSpot[" + std::to_string(i) + "].lightDiffuse").c_str());
        m_shader.lightSpotSettings[i].lightSpecularLoc = m_shader.m_shader->getParamLocation(std::string("lightsSpot[" + std::to_string(i) + "].lightSpecular").c_str());
        m_shader.lightSpotSettings[i].cutOff = m_shader.m_shader->getParamLocation(std::string("lightsSpot[" + std::to_string(i) + "].cutOff").c_str());
        m_shader.lightSpotSettings[i].direction = m_shader.m_shader->getParamLocation(std::string("lightsSpot[" + std::to_string(i) + "].direction").c_str());
    }
    m_shader.texture = m_shader.m_shader->getParamLocation("texture1");
    m_shader.cubemapShader->render();
    m_shader.mvLocCubemap = m_shader.cubemapShader->getParamLocation("modelview");
    m_shader.projCubemap = m_shader.cubemapShader->getParamLocation("projection");

}

void ShaderSetup::setupFboShader()
{
    if (m_virtual) {
        m_sizeXFbo = m_shader.ovr->getHmdIdealHorizRes();
        m_sizeYFbo = m_shader.ovr->getHmdIdealVertRes();
    }
    else {
        m_sizeXFbo = 1920;
        m_sizeYFbo = 1080;
    }
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    // Copy data into VBOs:
    glGenBuffers(1, &m_shader.boxVertexVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_shader.boxVertexVbo);

    glm::vec2* boxPlane = new glm::vec2[4];
    boxPlane[0] = glm::vec2(0.0f, 0.0f);
    boxPlane[1] = glm::vec2(1920, 0.0f);
    boxPlane[2] = glm::vec2(0.0f, 1080);
    boxPlane[3] = glm::vec2(1920, 1080);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), boxPlane, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glm::vec2 texCoord[4];
    texCoord[0] = glm::vec2(0.0f, 0.0f);
    texCoord[1] = glm::vec2(1.0f, 0.0f);
    texCoord[2] = glm::vec2(0.0f, 1.0f);
    texCoord[3] = glm::vec2(1.0f, 1.0f);
    glGenBuffers(1, &m_shader.boxTexCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_shader.boxTexCoordVbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), texCoord, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    m_shader.passthroughShader->render();


    // Bind params:
    m_shader.passthroughShader->bind(0, "in_Position");
    m_shader.passthroughShader->bind(2, "in_TexCoord");

    m_shader.ptProjLoc = m_shader.passthroughShader->getParamLocation("projection");
    m_shader.ptMvLoc = m_shader.passthroughShader->getParamLocation("modelview");
    glGetIntegerv(GL_VIEWPORT, prevViewPort);
    for (int c = 0; c < OvVR::EYE_LAST; c++)
    {
        glGenTextures(1, &m_shader.fboTexId[c]);
        glBindTexture(GL_TEXTURE_2D, m_shader.fboTexId[c]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_sizeXFbo, m_sizeYFbo, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        m_shader.fbo[c] = new Fbo();
        m_shader.fbo[c]->bindTexture(0, Fbo::BIND_COLORTEXTURE, m_shader.fboTexId[c]);
        m_shader.fbo[c]->bindRenderBuffer(1, Fbo::BIND_DEPTHBUFFER, m_sizeXFbo, m_sizeYFbo);
        if (!m_shader.fbo[c]->isOk())
            std::cout << "[ERROR] Invalid FBO" << std::endl;
    }
    Fbo::disable();
    glViewport(0, 0, prevViewPort[2], prevViewPort[3]);
    glBindVertexArray(0);
}

int* ShaderSetup::viewport()
{
    return prevViewPort;
}

unsigned int ShaderSetup::vao()
{
    return m_vao;
}

unsigned int ShaderSetup::sizeXFbo()
{
    return m_sizeXFbo;
}

unsigned int ShaderSetup::sizeYFbo()
{
    return m_sizeYFbo;
}
