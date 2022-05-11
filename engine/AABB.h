#include "Mesh.h"

struct LIB_API Plan
{
	glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
	float     distance = 0.f;        // Distance with origin

	Plan() = default;

	Plan(const glm::vec3& p1, const glm::vec3& norm)
		: normal(glm::normalize(norm)),
		distance(glm::dot(normal, p1))
	{}

	float getSignedDistanceToPlan(const glm::vec3& point) const
	{
		return glm::dot(normal, point) - distance;
	}
};

struct LIB_API Frustum
{
	Plan topFace;
	Plan bottomFace;

	Plan rightFace;
	Plan leftFace;

	Plan farFace;
	Plan nearFace;
};

class LIB_API AABB {
public:
	bool collide(Mesh* a, Mesh* b, glm::mat4 model_a, glm::mat4 model_b);
	bool collideSphere(glm::vec4 pos, Mesh* a, glm::mat4 model_a,float distance);
	Frustum createFrustumFromCamera(glm::mat4 camera, float aspect, float fovY, float zNear, float zFar);
};
