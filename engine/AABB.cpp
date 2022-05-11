#include "AABB.h"

bool LIB_API AABB::collide(Mesh* a, Mesh* b, glm::mat4 model_a, glm::mat4 model_b)
{
    auto min = model_a * glm::vec4(a->getBBox().first, 1);
    auto max = model_a * glm::vec4(a->getBBox().second, 1);

    auto min_other = model_b * glm::vec4(b->getBBox().first, 1);
    auto max_other = model_b * glm::vec4(b->getBBox().second, 1);
    return (min.x <= max_other.x && max.x >= min_other.x) &&
        (min.y <= max_other.y && max.y >= min_other.y) &&
        (min.z <= max_other.z && max.z >= min_other.z);
}

bool LIB_API AABB::collideSphere(glm::vec4 pos, Mesh* a, glm::mat4 model_a,float distance)
{
	auto min = model_a * glm::vec4(a->getBBox().first, 1);
	auto max = model_a * glm::vec4(a->getBBox().second, 1);
	if (glm::abs(glm::distance(pos, min)) >= distance) {
		return true;
	}
	if (glm::abs(glm::distance(pos, max)) >= distance) {
		return true;
	}
	return false;
}

Frustum LIB_API AABB::createFrustumFromCamera(glm::mat4 camera, float aspect, float fovY, float zNear, float zFar)
{
	Frustum     frustum;
	const float halfVSide = zFar * tanf(fovY * .5f);
	const float halfHSide = halfVSide * aspect;
	auto front = glm::vec3(camera[2]);

	const glm::vec3 frontMultFar = zFar * front;
	auto pos = glm::vec3(camera[3]);
	auto up = glm::vec3(camera[1]);
	auto right = glm::vec3(camera[0]);
	frustum.nearFace = { pos + zNear * front, front };
	frustum.farFace = { pos + frontMultFar, -front };
	frustum.rightFace = { pos, glm::cross(up, frontMultFar + right * halfHSide) };
	frustum.leftFace = { pos, glm::cross(frontMultFar - right * halfHSide, up) };
	frustum.topFace = { pos, glm::cross(right, frontMultFar - up * halfVSide) };
	frustum.bottomFace = { pos, glm::cross(frontMultFar + up * halfVSide, right) };

	return frustum;
}
