#include <iostream>

#include "Primitives.h"
#include "../Utils.h"

// Define Intersection functions
float Sphere::Intersect(const Ray& ray)
{
	// Transform ray to sphere space
	glm::vec3 origin = Utils::ApplyPoint(ObjectToWorld, ray.Origin);
	glm::vec3 direction = Utils::ApplyVector(ObjectToWorld, ray.Direction);
	
	float a = glm::dot(direction, direction);
	float b = 2.0f * glm::dot(origin, direction);
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

	glm::vec3 origin = Utils::ApplyPoint(ObjectToWorld, ray.Origin);
	glm::vec3 direction = Utils::ApplyVector(ObjectToWorld, ray.Direction);

	if (direction.y == 0.0f) return -1.0f;

	float t = -origin.y / direction.y;

	if (t <= 1e-6) return -1.0f;

	glm::vec3 hitPoint = origin + direction * t;

	if (hitPoint.x < -width/2.0f || hitPoint.x > width/2.0f || hitPoint.z < -height/2.0f || hitPoint.z > height/2.0f )
		return -1.0f;

	return t;
}
