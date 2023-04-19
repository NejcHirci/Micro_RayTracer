#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "../PBR/Ray.h"
#include "../Utils.h"

// Primitives
struct Shape
{
	Shape() = default;

	virtual float Intersect(const Ray& ray) = 0;
	virtual float Area() = 0;


	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 90.0f);
	glm::mat4 ObjectToWorld = glm::mat4(0.0f);
	glm::mat4 WorldToObject = glm::mat4(0.0f);
	int MaterialIndex = 0;

protected:
	bool SameHemisphere(glm::vec3 norm) { return glm::dot(Normal, norm) > 0; }

protected:
	glm::vec3 Normal = glm::vec3(0.0f, 1.0f, 0.0f);
};

struct Sphere : public Shape
{
	Sphere(glm::vec3 pos, glm::vec3 rot)
	{
		Rotation = rot;
		Position = pos;
		Normal = Utils::ApplyNormal(ObjectToWorld, Normal);
		ObjectToWorld = Utils::BuildRotateTranslate(Rotation, Position);
		WorldToObject = glm::inverse(ObjectToWorld);
	}

	float Intersect(const Ray& ray) override;
	float Area() { return 4 * glm::pi<float>() * Radius * Radius; }
	float Radius = 0.5f;
};

struct Quad : public Shape
{
	Quad(glm::vec3 pos, glm::vec3 rot)
	{
		Rotation = rot;
		Position = pos;
		Normal = Utils::ApplyNormal(ObjectToWorld, Normal);
		ObjectToWorld = Utils::BuildRotateTranslate(Rotation, Position);
		WorldToObject = glm::inverse(ObjectToWorld);
	}

	float Intersect(const Ray& ray) override;
	float Area() { return width * height; }
	
	float width = 10.0f;
	float height = 10.0f;
};
