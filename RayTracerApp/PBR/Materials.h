#pragma once

#include <glm/glm.hpp>

// Materials

struct Material
{
public:
	virtual glm::vec3 EvaluateBSDF(glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) = 0;
	// This is used for importance sampling ri
	virtual glm::vec3 SampleBSDF(glm::vec3 ro) = 0;

public:
	bool IsSpecular = false; 
	glm::vec3 Albedo;
};


struct Lambert : public Material
{
public:
	glm::vec3 EvaluateBSDF(glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) override;
	glm::vec3 SampleBSDF(glm::vec3 ro) override;
};

struct OrenNayar : public Material
{
public:
	glm::vec3 EvaluateBSDF(glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) override;
	glm::vec3 SampleBSDF(glm::vec3 ro) override;

public:
	float Roughness;
};

// TODO:  Support for light sampling with specular materials.
// TODO : Implement a transmitting BSDF model(glass, water, etc.) with support for specular transmission and caustics.
//        Use Fresnel's equations for determining the amount of transmitted light and Implement Snell's Law for refraction.
// TODO : Implement an anisotropic BRDF material like Aluminium.