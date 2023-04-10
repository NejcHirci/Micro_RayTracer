#include <glm\glm.hpp>

#include "Scene.h"
#include <iostream>

// TODO: Implement physically based spherical lights and the Oren-Nayar material model.

/*
* OPTIONAL
* TODO: support for light sampling with specular materials.
* TODO: Implement a transmitting BSDF model (glass, water, etc.) with support for specular transmission and caustics. 
*		Use Fresnel's equations for determining the amount of transmitted light and Implement Snell's Law for refraction.
* TODO: Implement an anisotropic BRDF material like Aluminium.
*/

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

float Plane::Intersect(const Ray& ray)
{
	float denom = glm::dot(ray.Direction, Normal);
	if (denom > 1e-6) {
		float t = glm::dot(Position - ray.Direction, Normal) / denom;
		return t;
	}
	return -1.0f;
}