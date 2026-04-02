/**
 * @file    material.cpp
 * @brief   Material class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

 // FreeGlut:
#include <GL/freeglut.h>

namespace Eng
{

	////////////////////
	// Material CLASS //
	////////////////////

	Material::Material(std::string name, glm::vec4 emission, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float shiness)
		: Object(name),
		emission(emission),
		ambient(ambient),
		diffuse(diffuse),
		specular(specular),
		shininess(shiness),
		texture(nullptr),
		shader(nullptr),
		matEmissionLoc(-1),
		matAmbientLoc(-1),
		matDiffuseLoc(-1),
		matSpecularLoc(-1),
		matShininessLoc(-1),
		hasTextureLoc(-1)
	{}

	Material::~Material()
	{
		delete texture;
	}

	void Material::render(glm::mat4 modelview)
	{
		if (shader)
			shader->render();

		Shader* shader = Shader::getCurrentInstance();

		if (texture)
			texture->render(modelview);
	}

	void Material::loadShaderParams(Eng::Shader* shader)
	{
		if (!shader)
			return;

		matEmissionLoc = shader->getParamLocation("matEmission");
		matAmbientLoc = shader->getParamLocation("matAmbient");
		matDiffuseLoc = shader->getParamLocation("matDiffuse");
		matSpecularLoc = shader->getParamLocation("matSpecular");
		matShininessLoc = shader->getParamLocation("matShininess");
		hasTextureLoc = shader->getParamLocation("hasTexture");
	}

	void Material::renderShader(Eng::Shader* shader)
	{
		if (!shader)
			return;

		shader->setVec3(matEmissionLoc, emission);
		shader->setVec3(matAmbientLoc, ambient);
		shader->setVec3(matDiffuseLoc, diffuse);
		shader->setVec3(matSpecularLoc, specular);
		shader->setFloat(matShininessLoc, shininess);
		shader->setBool(hasTextureLoc, texture);
	}

	glm::vec4 Material::getEmission() const
	{
		return emission;
	}

	glm::vec4 Material::getAmbient() const
	{
		return ambient;
	}

	glm::vec4 Material::getDiffuse() const
	{
		return diffuse;
	}

	glm::vec4 Material::getSpecular() const
	{
		return specular;
	}

	float Material::getShininess() const
	{
		return shininess;
	}

	Eng::Texture* Material::getTexture() const
	{
		return texture;
	}

	Eng::Shader* Material::getShader() const
	{
		return shader;
	}

	void Material::setEmission(const glm::vec4& emission_)
	{
		emission = emission_;
	}

	void Material::setAmbient(const glm::vec4& ambient_)
	{
		ambient = ambient_;
	}

	void Material::setDiffuse(const glm::vec4& diffuse_)
	{
		diffuse = diffuse_;
	}

	void Material::setSpecular(const glm::vec4& specular_)
	{
		specular = specular_;
	}

	void Material::setShininess(float shininess_)
	{
		shininess = shininess_;
	}

	void Material::setTexture(Eng::Texture* texture_)
	{
		texture = texture_;
	}

	void Material::setShader(Eng::Shader* shader_)
	{
		shader = shader_;
	}

}; // end of namespace Eng::
