#include "Mesh.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_inverse.hpp>


LIB_API Mesh::Mesh() :m_has_shadows{ true } {
    matrix_shadow = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
}

LIB_API Mesh::~Mesh() {

}

void LIB_API Mesh::material(std::shared_ptr<Material> material) {
    m_material = material;
}

std::shared_ptr<Material LIB_API> Mesh::material() const {
    return m_material;
}

void LIB_API Mesh::LOD(int LOD) {
    m_LOD = LOD;
}

int LIB_API Mesh::LOD() const {
    return m_LOD;
}

void LIB_API Mesh::vertices(LODdata vertices)
{
    m_vertices = std::make_shared<LODdata>(vertices);
    int lods = 0;
    auto m_lod_vertices = m_vertices->lod.at(lods).vertices;
    auto m_lod_normal = &m_vertices->lod.at(lods).normal;
    auto m_lod_uv = &m_vertices->lod.at(lods).uv;
    auto m_lod_faces = &m_vertices->lod.at(lods).faces;
    std::vector<unsigned int> faces;
    for (int i = 0; i < m_lod_faces->size(); i++) {
        for (int j = 0; j < 3; j++) {
            faces.push_back(m_lod_faces->at(i).at(j));
        }
    }
    // Init VAO:   
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &vertexVbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVbo);
    // Copy the vertex data from system to video memory:
    glBufferData(GL_ARRAY_BUFFER, m_lod_vertices.size() * sizeof(glm::vec3),
        m_lod_vertices.data(), GL_STATIC_DRAW);


    glGenBuffers(1, &faceVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVbo);
    // Copy the face index data from system to video memory:
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        m_lod_faces->size() * 3 * sizeof(unsigned int),
        faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &normalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, normalVbo);
    // Copy the normal data from system to video memory:
    glBufferData(GL_ARRAY_BUFFER, m_lod_normal->size() * sizeof(glm::vec3),
        m_lod_normal->data(), GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &uvVbo);
    glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
    // Copy the uv data from system to video memory:
    glBufferData(GL_ARRAY_BUFFER, m_lod_uv->size() * sizeof(glm::vec2),
        m_lod_uv->data(), GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    face_size = m_lod_faces->size();
    matrix_shadow = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f));
}

LODdata LIB_API Mesh::vertices() const
{
    return *m_vertices;
}

void LIB_API Mesh::render(glm::mat4 modelView, ShaderSettings &shader) {
    int lods = 0;
    shader.m_shader->setMatrix(shader.modelview, modelView);

    shader.m_shader->setMatrix3(shader.normalMatLoc, glm::inverseTranspose(glm::mat3(modelView)));

    if (m_material) {
        m_material->render(modelView,shader);
    }

    glBindVertexArray(m_VAO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVbo);
    glDrawElements(GL_TRIANGLES, face_size * 3, GL_UNSIGNED_INT, nullptr);

}

void LIB_API Mesh::render_shadow(glm::mat4 modelViewShadow,ShaderSettings &shader) {
    shader.m_shader->setMatrix(shader.modelview, modelViewShadow);
    shader.m_shader->setVec3(shader.matAmbientLoc, glm::vec3(0.0f,0.0f,0.0f));
    shader.m_shader->setVec3(shader.matDiffuseLoc, glm::vec3(0.0f, 0.0f, 0.0f));
    shader.m_shader->setVec3(shader.matSpecularLoc, glm::vec3(0.0f, 0.0f, 0.0f));
    glBindVertexArray(m_VAO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVbo);
    glDrawElements(GL_TRIANGLES, face_size * 3, GL_UNSIGNED_INT, nullptr);
}

void LIB_API Mesh::shadow(const bool& shadow)
{
    m_has_shadows = shadow;
}

bool LIB_API Mesh::shadow() const
{
    return m_has_shadows;
}

void LIB_API Mesh::VAO(int vao)
{
    m_VAO = vao;
}

int LIB_API Mesh::VAO() const
{
    return m_VAO;
}

Mesh LIB_API* Mesh::clone()
{
    return new Mesh(*this);
}

std::pair<glm::vec3, glm::vec3> LIB_API Mesh::getBBox()
{
    return {m_vertices->bBoxMin,m_vertices->bBoxMax};
}
