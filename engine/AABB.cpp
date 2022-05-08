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
