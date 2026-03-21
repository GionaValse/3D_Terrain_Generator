/**
 * @file    spotlight.cpp
 * @brief   SpotLight class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

 // Freeglut:
#include <GL/freeglut.h>

namespace Eng
{

	//////////////////
	// SpotLight CLASS //
	//////////////////

	SpotLight::SpotLight(
		std::string name,
		glm::mat4 matrix,
		glm::vec3 direction,
		float cutoff,
		float influenceRadius
	)
		: Eng::Light(name, matrix),
		direction(direction),
		influenceRadius(influenceRadius),
		lightTypeLoc(-1),
		spotCutoffCosLoc(-1),
		spotExponentLoc(-1),
		lightDirectionLoc(-1)
	{
		setCutoff(cutoff);
	}

	SpotLight::~SpotLight()
	{}

	void SpotLight::loadShaderParams(Eng::Shader* shader)
	{
		if (!shader)
			return;

		Light::loadShaderParams(shader);

		std::string prefix = "[" + std::to_string(getLightID()) + "]";

		lightTypeLoc = shader->getParamLocation(("lightType" + prefix).c_str());
		spotCutoffCosLoc = shader->getParamLocation(("spotCutoffCos" + prefix).c_str());
		spotExponentLoc = shader->getParamLocation(("spotExponent" + prefix).c_str());
		lightDirectionLoc = shader->getParamLocation(("lightDirection" + prefix).c_str());
	}

	void SpotLight::renderShader(Eng::Shader* shader, glm::mat4 modelview)
	{
		if (!shader)
			return;

		Light::renderShader(shader, modelview);

		glm::vec3 eyeSpaceDirection = glm::vec3(modelview * glm::vec4(direction, 0.0f));
		glm::vec3 normalizedDirection = glm::normalize(eyeSpaceDirection);

		float cutoffCos = glm::cos(glm::radians(cutoff));

		shader->setInt(lightTypeLoc, 2);
		shader->setFloat(spotCutoffCosLoc, cutoffCos);
		shader->setFloat(spotExponentLoc, influenceRadius);
		shader->setVec3(lightDirectionLoc, normalizedDirection);
	}

	float SpotLight::getCutoff() const
	{
		return cutoff;
	}

	float SpotLight::getInfluenceRadius() const
	{
		return influenceRadius;
	}

	glm::vec3 SpotLight::getDirection() const
	{
		return direction;
	}

	void SpotLight::setCutoff(float cutoff_)
	{
		cutoff = (cutoff_ < 0 || cutoff_ > 90) ? 0 : cutoff_;
	}

	void SpotLight::setInfluenceRadius(float radius)
	{
		influenceRadius = radius;
	}

	void SpotLight::setDirection(glm::vec3 dir)
	{
		direction = dir;
	}

}; // end of namespace Eng::
