#include <iostream>

#include "Primitives.h"
#include "../Utils.h"

// Define Intersection functions
float Sphere::Intersect(const Ray& ray)
{
	glm::vec3 origin = ray.Origin - Position;

	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(origin, ray.Direction);
	float c = glm::dot(origin, origin) - Radius * Radius;

	// Discriminant
	float d = b * b - 4.0f * a * c;
	if (d < 0.0f) {
		return -1.0f;
	}

	// Closest intersection
	float t0 = (-b - glm::sqrt(d)) / (2.0f * a);

	return t0;
}

float Quad::Intersect(const Ray& ray)
{
	// Jump out if not facing the plane
	if (glm::dot(Normal, -ray.Direction) < 0.0f) return -1.0f;

	glm::vec3 origin = Utils::ApplyPoint(WorldToObject, ray.Origin);
	glm::vec3 direction = Utils::ApplyVector(WorldToObject, ray.Direction);

	if (direction.z == 0.0f) return -1.0f;

	float t = -origin.y / direction.y;

	if (t <= 1e-6) return -1.0f;

	glm::vec3 hitPoint = origin + direction * t;

	if (hitPoint.x < -width/2.0f || hitPoint.x > width/2.0f || hitPoint.z < -height/2.0f || hitPoint.z > height/2.0f )
		return -1.0f;

	return t;
}
