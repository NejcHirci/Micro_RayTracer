#pragma once

#include <glm/glm.hpp>

#include "../Geometry/Primitives.h"

struct Light
{
	// Sample a point on shape
	virtual glm::vec3 SamplePoint() = 0;
	virtual glm::vec3 SampleLightRay(glm::vec3 point) = 0;

	// Evaluate the light color at a point
	virtual glm::vec3 EvaluateLight(glm::vec3 point) = 0;

public:
	Shape* Shape;
	glm::vec3 Color;
	float Intensity = 1.0f;
};

struct DiffuseAreaLight : public Light 
{
public:
	glm::vec3 SamplePoint() override;
	glm::vec3 SampleLightRay(glm::vec3 point) override;
	// Evaluate the light color at a point
	glm::vec3 EvaluateLight(glm::vec3 point) override;
	
};