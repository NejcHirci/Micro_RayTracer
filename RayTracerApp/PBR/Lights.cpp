#include <iostream>

#include "../Utils.h"
#include "Lights.h"
#include "../Renderer.h"

glm::vec3 DiffuseAreaLight::LightEmission(glm::vec3 norm, glm::vec3 wi) 
{
	return (glm::dot(norm, wi) > 0) ? Color * Intensity : glm::vec3(0.0f);
}

Ray DiffuseAreaLight::SampleLightRay(glm::vec3 point, float r)
{
	// Generate a random point on the light source
	glm::vec3 randP = Utils::UniformSampleSphere(r);

	// Calculate the light ray
	Ray ray;
	ray.Origin = point;
	ray.Direction = glm::normalize(randP - point);

	return ray;
}

float DiffuseAreaLight::CalculatePdf(glm::vec3 objNormal, glm::vec3 objPosition, glm::vec3 lightNorm, glm::vec3 lightPos)
{
	float distance = glm::length(Shape->Position - objPosition);
	distance *= distance;

	float cosAlpha = glm::dot(lightNorm, objNormal);

	float area = Shape->Area();

	if (cosAlpha == 0.0f) return 0.0f;

	return distance / (glm::pi<float>() * cosAlpha);
}

