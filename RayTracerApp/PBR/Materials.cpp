#include <iostream>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/perpendicular.hpp>

#include <Walnut/Random.h>

#include "../Utils.h"
#include "Materials.h"

float Material::CalculatePdf(glm::vec3 wo, glm::vec3 wi)
{
	// Because we are using only cosine weighted sampling, the 
	// cosine theta factor cancels out
	return glm::dot(wo, wi) > 0 ? glm::one_over_pi<float>() : 0.0f;
}

glm::vec3 Lambert::EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	glm::vec3 sphericalRo = Utils::CartesianToSpherical(ro);
	glm::vec3 sphericalRi = Utils::CartesianToSpherical(ri);

	return Albedo * glm::one_over_pi<float>();
}


glm::vec3 Lambert::SampleBSDF(glm::vec3 norm, glm::vec3 ri)
{
	glm::vec3 v = Utils::CosineSampleHemisphere(norm);

	// Convert to world space using norm
	glm::vec3 tangent = glm::vec3(0.0f);
	
	if (norm == ri) return glm::vec3(0.0f);

	tangent = glm::normalize(glm::cross(norm, ri));
	glm::vec3 bitangent = glm::normalize(glm::cross(norm, tangent));
	glm::mat3 worldToLocal = glm::transpose(glm::mat3(tangent, norm, bitangent));
	glm::mat3 localToWorld = glm::inverse(worldToLocal);
	v = localToWorld * v;

	return glm::normalize(v);
}


float Lambert::CalculatePdf(glm::vec3 ro, glm::vec3 ri)
{
	// Because we are using only cosine weighted sampling, the 
	// cosine theta factor cancels out
	return glm::dot(ro, ri) > 0 ? glm::one_over_pi<float>() : 0.0f;
}

glm::vec3 OrenNayar::EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	// Convert to ro and ri to local space
	glm::vec3 tangent = glm::cross(worldNormal, ri);
	glm::vec3 bitangent = glm::cross(worldNormal, tangent);
	glm::mat3 worldToLocal = glm::transpose(glm::mat3(tangent, worldNormal, bitangent));

	ro = worldToLocal * ro;
	ri = worldToLocal * ri;

	glm::vec3 sphericalRo = Utils::CartesianToSpherical(ro);
	glm::vec3 sphericalRi = Utils::CartesianToSpherical(ri);

	float sigma2 = Roughness * Roughness;

	float A = 1.0f - (sigma2 / (2.0f * (sigma2 + 0.33f)));
	float B = 0.45f * sigma2 / (sigma2 + 0.09f);

	float sinThetaI = glm::sin(sphericalRi.y);
	float sinThetaO = glm::sin(sphericalRo.y);

	float maxCos = 0.0f;
	if (sinThetaI > 1e-4f && sinThetaO > 1e-4f) {
		float sinPhiI = glm::sin(sphericalRi.z);
		float cosPhiI = glm::cos(sphericalRi.z);
		float sinPhiO = glm::sin(sphericalRo.z);
		float cosPhiO = glm::cos(sphericalRo.z);
		float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
		maxCos = glm::max(0.0f, dCos);
	}

	float sinAlpha, tanBeta;
	if (glm::abs(sphericalRi.y) > glm::abs(sphericalRo.x)) {
		sinAlpha = sinThetaO;
		tanBeta = sinThetaI / glm::abs(glm::cos(sphericalRi.y));
	}
	else {
		sinAlpha = sinThetaI;
		tanBeta = sinThetaO / glm::abs(glm::cos(sphericalRo.y));
	}

	return Albedo * glm::one_over_pi<float>() * (A + B * maxCos * sinAlpha * tanBeta);
}


glm::vec3 OrenNayar::SampleBSDF(glm::vec3 norm, glm::vec3 ri) 
{
	// In local space
	glm::vec3 v = Utils::CosineSampleHemisphere(norm);

	// Convert to world space using norm
	glm::vec3 tangent = glm::vec3(0.0f);
	tangent = glm::normalize(glm::cross(norm, ri));
	glm::vec3 bitangent = glm::normalize(glm::cross(norm, tangent));
	glm::mat3 worldToLocal = glm::transpose(glm::mat3(tangent, norm, bitangent));
	glm::mat3 localToWorld = glm::inverse(worldToLocal);
	v = localToWorld * v;

	return glm::normalize(v);
}

float OrenNayar::CalculatePdf(glm::vec3 wo, glm::vec3 wi)
{
	// Because we are using only cosine weighted sampling, the 
	// cosine theta factor cancels out
	return glm::dot(wo, wi) > 0 ? glm::one_over_pi<float>() : 0.0f;
}


glm::vec3 DielectricSpecular::EvaluateBSDF(Shape * shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri) { return glm::vec3(1.0f); }

float DielectricSpecular::CalculatePdf(glm::vec3 ro, glm::vec3 ri) { return 1.0f; }

glm::vec3 DielectricSpecular::SampleBSDF(glm::vec3 norm, glm::vec3 ri)
{
	// Check if we are entering or exiting the medium
	float refraction_ratio = glm::dot(norm, ri) > 0.0f ? 1.0f / ior : ior;

	float cosTheta = glm::min(glm::dot(norm, ri), 1.0f);
	float sinTheta = glm::sqrt(glm::max(0.0f, 1.0f - cosTheta * cosTheta));

	bool cannotRefract = refraction_ratio * sinTheta > 1.0f;
	glm::vec3 direction;

	if (cannotRefract || Reflectance(cosTheta, refraction_ratio) > Walnut::Random::Float()) {
		direction = glm::reflect(ri, norm);
	}
	else {
		direction = glm::refract(ri, norm, refraction_ratio);
	}

	return direction;
}

float DielectricSpecular::Reflectance(float cosTheta, float refraction_ratio)
{
	// Using Schilck's approximation for reflectance
	auto r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
	r0 = r0 * r0;
	return r0 + (1 - r0) * glm::pow(1.0f - cosTheta, 5.0f);
}

glm::vec3 SimpleMetal::EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	return Albedo;
}

glm::vec3 SimpleMetal::SampleBSDF(glm::vec3 norm, glm::vec3 ri)
{
	// Fuzzy reflection based on roughness
	glm::vec3 v = glm::reflect(ri, norm);
	v = glm::normalize(v);
	v = glm::normalize(v + Roughness * Utils::UniformSampleSphere(1.0f));
	return v;
}

float SimpleMetal::CalculatePdf(glm::vec3 ro, glm::vec3 ri)
{
	return glm::dot(ro, ri) > 0.0f ? 1.0f : 0.0f;
}
