#pragma once
#include "Node.h"
#include "Material.h"
#include "Camera.h"
#include <memory>
#include <vector>
#include <array>
#include <tuple>

struct LIB_API VerticesData {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec2> uv;
	std::vector<std::array<unsigned int, 3>> faces;
};

struct LIB_API LODdata {
	std::vector<VerticesData> lod;
	glm::vec3 bBoxMin;
	glm::vec3 bBoxMax;
};

class LIB_API Mesh : public Node {
public:
	Mesh();
	~Mesh();

	void material(std::shared_ptr<Material> material);
	std::shared_ptr<Material> material() const;

	void LOD(int LOD);
	int LOD() const;

	void vertices(LODdata vertices);
	LODdata vertices() const;

	//void render(std::shared_ptr<Object> camera) override;
	void render(glm::mat4 modelView, ShaderSettings &shader) override;

	void render_shadow(glm::mat4 modelViewShadow);
	glm::mat4 get_shadow_mat() { return matrix_shadow; };

	void shadow(const bool& shadow);
	bool shadow() const;
	void VAO(int vao);
	int VAO() const;
	virtual Mesh* clone() override;
	std::pair<glm::vec3, glm::vec3> getBBox();

private:
	std::shared_ptr<Material> m_material;
	int m_LOD;
	std::shared_ptr<LODdata> m_vertices;
	bool m_has_shadows;
	glm::mat4 matrix_shadow;

	unsigned int m_VAO;
	unsigned int normal;
	unsigned int uv;
	unsigned int faceVbo;

	unsigned int vertexVbo;
	unsigned int normalVbo;
	unsigned int uvVbo;

	int face_size;
};
