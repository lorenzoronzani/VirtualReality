#pragma once
#include "Node.h"
#include "Camera.h"

class OvLight
{
public:
    enum class Subtype : int
    {
        OMNI = 0,
        DIRECTIONAL,
        SPOT,
        LAST,
    };
};

struct LightSettings {
    OvLight::Subtype light_type;
    glm::vec3 color;
    float radius;
    glm::vec3 direction;
    float cutoff;
    float spot_exponent;
    int light_number;
};

class LIB_API Light : public Node {
public:
	Light() = default;
	~Light() = default;

	void settings(const LightSettings& lightSettings);
	LightSettings settings() const;

	//void render(std::shared_ptr<Object> camera);
	void render(glm::mat4 modelView, Shader &shader) override;

private:
	LightSettings m_settings;
};

