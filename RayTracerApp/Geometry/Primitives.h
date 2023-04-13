#pragma once

#include <glm/glm.hpp>

#include "../PBR/Ray.h"

// Primitives
struct Shape
{
	virtual float Intersect(const Ray& ray) = 0;

	glm::vec3 Position;
	int MaterialIndex = 0;
};

struct Sphere : public Shape
{
	float Intersect(const Ray& ray) override;
	float Radius = 0.5f;
};

struct Plane : public Shape
{
	float Intersect(const Ray& ray) override;
	glm::vec3 Normal;
};
