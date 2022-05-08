#include "Mesh.h"

class LIB_API AABB {
public:
	bool collide(Mesh* a, Mesh* b, glm::mat4 model_a, glm::mat4 model_b);
};
