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
}

LODdata LIB_API Mesh::vertices() const
{
    return *m_vertices;
}

void LIB_API Mesh::render(glm::mat4 modelView, ShaderSettings &shader) {
    int lods = 0;
    shader.m_shader->setMatrix3(shader.normalMatLoc, glm::inverseTranspose(glm::mat3(modelView)));

    if (m_material) {
        m_material->render(modelView,shader);
    }
    auto m_lod_faces = &m_vertices->lod.at(lods).faces;
    auto size = m_lod_faces->size();

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVbo);
    glDrawElements(GL_TRIANGLES, m_lod_faces->size() * 3, GL_UNSIGNED_INT, nullptr);


    //if (shadow()) {
    //std::cout << this->name() << " ho l'ombra" << std::endl;
        //glm::mat4 matrix_shadow_new = matrix_shadow * getFinalMatrix();
        //glm::mat4 matrix_shadow_new = modelView * matrix_shadow;

        //view*model
        //matrix_shadow_new = dynamic_cast<Camera*>(camera.get())->inverseCamera() * matrix_shadow_new;
        //matrix_shadow_new = modelView * matrix_shadow_new;
        //glLoadMatrixf(glm::value_ptr(matrix_shadow_new));
        //render_shadow();
    //}
}

void LIB_API Mesh::render_shadow(glm::mat4 modelViewShadow) {
    glLoadMatrixf(glm::value_ptr(modelViewShadow));

    int lods = 0;

    glm::vec3 color_shadow;
    color_shadow = glm::vec3(0.0f, 0.0f, 0.0f);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    auto m_lod_vertices = &m_vertices->lod.at(lods).vertices;
    auto m_lod_faces = &m_vertices->lod.at(lods).faces;

    auto size = m_lod_faces->size();

    //Renderizzo l'ombra
    glBegin(GL_TRIANGLES);
    for (int j = 0; j < size; j++) {
        glColor3fv(glm::value_ptr(color_shadow));

        auto m_lod_faces_j = m_lod_faces->at(j);

        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_j.at(0))));
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_j.at(1))));
        glVertex3fv(glm::value_ptr(m_lod_vertices->at(m_lod_faces_j.at(2))));
    }
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
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
