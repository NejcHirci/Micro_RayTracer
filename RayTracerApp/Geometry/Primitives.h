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
	virtual glm::vec3 GetNormal(glm::vec3 p) = 0;


	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 90.0f);
	glm::mat4 ObjectToWorld = glm::mat4(0.0f);
	glm::mat4 WorldToObject = glm::mat4(0.0f);
	int MaterialIndex = 0;
};

struct Sphere : public Shape
{
	Sphere(glm::vec3 pos, glm::vec3 rot)
	{
		Rotation = rot;
		Position = pos;
	}

	float Intersect(const Ray& ray) override;
	float Area() { return 4 * glm::pi<float>() * Radius * Radius; }
	glm::vec3 GetNormal(glm::vec3 p) { return glm::normalize(p - Position); };
	
	float Radius = 0.5f;
};

struct Quad : public Shape
{
	Quad(glm::vec3 pos, glm::vec3 rot)
	{
		Rotation = rot;
		Position = pos;
		ObjectToWorld = Utils::BuildRotateTranslate(Rotation, Position);
		WorldToObject = glm::inverse(ObjectToWorld);
		Normal = Utils::ApplyNormal(ObjectToWorld, Normal);
	}

	float Intersect(const Ray& ray) override;
	float Area() { return width * height; }
	glm::vec3 GetNormal(glm::vec3 p) { return Normal; };
	
	float width = 10.0f;
	float height = 10.0f;
	glm::vec3 Normal = { 0.0f, 1.0f, 0.0f };
};
