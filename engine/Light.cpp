#include "Light.h"
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void LIB_API Light::settings(const LightSettings& lightSettings){
	m_settings = lightSettings;
}

LightSettings LIB_API Light::settings() const{
	return m_settings;
}

void LIB_API Light::render(glm::mat4 modelView, ShaderSettings &shader){
	
	//model_view = dynamic_cast<Camera*>(camera.get())->inverseCamera() * getFinalMatrix();

	//Switch per tipologia luce
	switch ((OvLight::Subtype)m_settings.light_type)
	{
		case OvLight::Subtype::DIRECTIONAL: {

			break;
		}
		case OvLight::Subtype::OMNI: {
			shader.m_shader->setVec3(shader.lightSettings[m_settings.light_number].lightAmbientLoc, glm::vec3(0.1f, 0.1f, 0.1f));
			shader.m_shader->setVec3(shader.lightSettings[m_settings.light_number].lightDiffuseLoc, m_settings.color);
			shader.m_shader->setVec3(shader.lightSettings[m_settings.light_number].lightSpecularLoc, m_settings.color);
			shader.m_shader->setVec3(shader.lightSettings[m_settings.light_number].lightPositionLoc, glm::vec3(modelView * glm::vec4(1.0f)));

			break;
		}
		case OvLight::Subtype::SPOT: {
			shader.m_shader->setVec3(shader.lightSpotSettings[m_settings.light_number].lightAmbientLoc, glm::vec3(0.1f, 0.1f, 0.1f));
			shader.m_shader->setVec3(shader.lightSpotSettings[m_settings.light_number].lightDiffuseLoc, m_settings.color);
			shader.m_shader->setVec3(shader.lightSpotSettings[m_settings.light_number].lightSpecularLoc, m_settings.color);
			shader.m_shader->setVec3(shader.lightSpotSettings[m_settings.light_number].lightPositionLoc, glm::vec3(modelView * glm::vec4(1.0f)));
			shader.m_shader->setFloat(shader.lightSpotSettings[m_settings.light_number].cutOff, m_settings.cutoff);
			shader.m_shader->setVec3(shader.lightSpotSettings[m_settings.light_number].direction, m_settings.direction);
			shader.m_shader->setFloat(shader.lightSpotSettings[m_settings.light_number].outerCutOff, m_settings.radius);

			break;
		}
	}

}

Light LIB_API* Light::clone()
{
	return new Light(*this);
}