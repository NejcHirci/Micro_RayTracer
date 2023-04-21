#pragma once

#include <glm/glm.hpp>

#include "../Geometry/Primitives.h"


struct Light
{
	virtual glm::vec3 LightEmission(glm::vec3 norm, glm::vec3 wi) = 0;
	virtual Ray SampleLightRay(glm::vec3 point) = 0;
	virtual float CalculatePdf(glm::vec3 objectNorm, glm::vec3 objectPosition, glm::vec3 lightNorm, glm::vec3 lightPos) = 0;

public:
	Shape* Shape;
	glm::vec3 Color;
	float Intensity = 1.0f;
};

struct DiffuseAreaLight : public Light 
{
public:
	glm::vec3 LightEmission(glm::vec3 norm, glm::vec3 wi) override;
	Ray SampleLightRay(glm::vec3 point) override;
	float CalculatePdf(glm::vec3 rayDirection, glm::vec3 objectPosition, glm::vec3 lightNorm, glm::vec3 lightPos) override;
};