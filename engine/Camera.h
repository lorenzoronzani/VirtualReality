#pragma once
#include "Node.h"

class LIB_API Camera : public Node{
public:
	Camera();
	~Camera();

	glm::mat4 inverseCamera() const;
	void setTransformation(const glm::mat4& transformation) override;
	void render(std::shared_ptr<Object> camera);

private:
	glm::mat4 m_camera_inverse;
	glm::mat4 m_camera;
};
