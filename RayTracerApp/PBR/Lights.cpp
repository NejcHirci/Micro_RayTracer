#include <iostream>

#include "../Utils.h"
#include "Lights.h"
#include "../Renderer.h"

glm::vec3 DiffuseAreaLight::LightEmission(glm::vec3 norm, glm::vec3 wi) 
{
	return Color * Intensity;
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

float DiffuseAreaLight::CalculatePdf(glm::vec3 rayDirection, glm::vec3 objectPosition, glm::vec3 lightNorm, glm::vec3 lightPos)
{
	float distance = glm::length(lightPos - objectPosition);
	distance *= distance;

	float cosAlpha = glm::dot(lightNorm, rayDirection);

	float area = Shape->Area();

	return 1 / distance;
}

