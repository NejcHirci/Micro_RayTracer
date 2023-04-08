#include <glm\glm.hpp>

#include "Scene.h"
#include <iostream>
// Define Intersection functions

float Sphere::Intersect(const Ray& ray)
{
	glm::vec3 origin = ray.Origin - Position;
	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(origin, ray.Direction);
	float c = glm::dot(origin, origin) - Radius * Radius;

	// Discriminant
	float d = b * b - 4.0f * a * c;
	if (d <= 0.0f) {
		return -1.0f;
	}

	// Closest intersection
	float t0 = (-b - glm::sqrt(d)) / (2.0f * a);

	return t0;
}

float Plane::Intersect(const Ray& ray)
{
	float denom = glm::dot(ray.Direction, Normal);
	if (denom > 1e-6) {
		float t = glm::dot(Position - ray.Direction, Normal) / denom;
		return t;
	}
	return -1.0f;
}