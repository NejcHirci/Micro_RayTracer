#pragma once

#include <glm/glm.hpp>

#include "../Geometry/Primitives.h"

// TODO : Implement a transmitting BSDF model(glass, water, etc.) with support for specular transmission and caustics.
//        Use Fresnel's equations for determining the amount of transmitted light and Implement Snell's Law for refraction.

// TODO : Implement an anisotropic BRDF material like Aluminium.

struct Material
{
public:
	virtual glm::vec3 EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) = 0;
	// This is used for importance sampling ri
	virtual glm::vec3 SampleBSDF(glm::vec3 norm, glm::vec3 ri) = 0;
	virtual float CalculatePdf(glm::vec3 ro, glm::vec3 ri);

public:
	glm::vec3 Albedo;
};


struct Lambert : public Material
{
public:
	glm::vec3 EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) override;
	glm::vec3 SampleBSDF(glm::vec3 norm, glm::vec3 ri) override;
	float CalculatePdf(glm::vec3 ro, glm::vec3 ri) override;
};

struct OrenNayar : public Material
{
public:
	glm::vec3 EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) override;
	glm::vec3 SampleBSDF(glm::vec3 norm, glm::vec3 ri) override;
	float CalculatePdf(glm::vec3 ro, glm::vec3 ri) override;

public:
	float Roughness;
};

struct DielectricSpecular : public Material
{
public:
	glm::vec3 EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) override;
	glm::vec3 SampleBSDF(glm::vec3 norm, glm::vec3 ri) override;
	float CalculatePdf(glm::vec3 ro, glm::vec3 ri) override;

public:
	float ior;

private:
	float Reflectance(float cosTheta, float refraction_ratio);
};

struct SimpleMetal : public Material
{
public:
	glm::vec3 EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) override;
	glm::vec3 SampleBSDF(glm::vec3 norm, glm::vec3 ri) override;
	float CalculatePdf(glm::vec3 ro, glm::vec3 ri) override;

public:
	float Roughness = 0.3f;
};