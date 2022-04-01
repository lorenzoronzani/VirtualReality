#pragma once
#include "Node.h"

class LIB_API Camera : public Node{
public:
	Camera() = default;
	~Camera() = default;

	glm::mat4 inverseCamera() const;
	void setTransformation(const glm::mat4& transformation) override;
	void render(glm::mat4 modelView, ShaderSettings &shader);

private:
	glm::mat4 m_camera_inverse;
	glm::mat4 m_camera;
};
