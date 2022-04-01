#include "Camera.h"

glm::mat4 LIB_API Camera::inverseCamera() const{
    return m_camera_inverse;
}

void LIB_API Camera::setTransformation(const glm::mat4& transformation)
{
    m_camera = transformation;
    m_camera_inverse = glm::inverse(m_camera);
}

void LIB_API Camera::render(glm::mat4 modelView, Shader &shader){

}
