#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "Ray.h"

// Primitives
struct Shape
{
	virtual float Intersect(const Ray& ray) = 0;

	glm::vec3 Position;
	glm::quat Rotation;
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
	glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);
};

// Lights
struct Light
{
	glm::vec3 Color;
	Shape* shape;
	float intensity = 1.0f;
};

// Materials

struct Material
{
	bool m_IsSpecular = false;
	glm::vec3 Albedo;
};


struct MicrofacetBRDF
{
	float Roughness = 0.5f;
	float Metallic = 0.0f;

	glm::vec3 albedo;
	glm::vec3 normal;
};

// Scene
struct Scene
{
	std::vector<Light> lights;
	std::vector<Shape*> shapes;
	std::vector<Material> materials;
};
