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
	/*
	//Prendo matrice camera
	glm::mat4 model_view;
	//model_view = dynamic_cast<Camera*>(camera.get())->inverseCamera() * getFinalMatrix();
	glLoadMatrixf(glm::value_ptr(modelView));

	//Switch per tipologia luce
	switch ((OvLight::Subtype)m_settings.light_type)
	{
		case OvLight::Subtype::DIRECTIONAL: {
			glm::vec4 directional(0.0f, 0.0f, 0.0f, 0.0f);

			glLightfv(GL_LIGHT0 + m_settings.light_number, GL_POSITION, glm::value_ptr(directional));
			break;
		}
		case OvLight::Subtype::OMNI: {
			glm::vec4 omni(0.0f, 0.0f, 0.0f, 1.0f);

			glLightfv(GL_LIGHT0 + m_settings.light_number, GL_POSITION, glm::value_ptr(omni));
			glLightfv(GL_LIGHT0 + m_settings.light_number, GL_SPOT_CUTOFF, &m_settings.cutoff);
			break;
		}
		case OvLight::Subtype::SPOT: {
			glm::vec4 spot(0.0f, 0.0f, 0.0f, 1.0f);

			glLightfv(GL_LIGHT0 + m_settings.light_number, GL_POSITION, glm::value_ptr(spot));
			glLightfv(GL_LIGHT0 + m_settings.light_number, GL_SPOT_CUTOFF, &m_settings.cutoff);
			glLightfv(GL_LIGHT0 + m_settings.light_number, GL_SPOT_DIRECTION, glm::value_ptr(m_settings.direction));
			glLightf(GL_LIGHT0 + m_settings.light_number, GL_SPOT_EXPONENT, m_settings.spot_exponent);

			break;
		}
	}

	glLightfv(GL_LIGHT0 + m_settings.light_number, GL_DIFFUSE, glm::value_ptr(m_settings.color));
	glLightfv(GL_LIGHT0 + m_settings.light_number, GL_SPECULAR, glm::value_ptr(m_settings.color));
	*/
	shader.m_shader->setVec3(shader.lightAmbientLoc, glm::vec3(1.0f, 1.0f, 1.0f));
	shader.m_shader->setVec3(shader.lightDiffuseLoc, glm::vec3(1.0f, 1.0f, 1.0f));
	shader.m_shader->setVec3(shader.lightSpecularLoc, glm::vec3(1.0f, 1.0f, 1.0f));
	shader.m_shader->setVec3(shader.lightPositionLoc, glm::vec3(modelView * glm::vec4(1.0f)));

	
}

Light LIB_API* Light::clone()
{
	return new Light(*this);
}