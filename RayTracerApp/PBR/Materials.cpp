#include <iostream>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/perpendicular.hpp>

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


glm::vec3 OrenNayar::EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
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
	if (glm::abs(sphericalRi.y - sphericalRo.y) > 1e-4f) {
		sinAlpha = glm::min(sphericalRi.y, sphericalRo.y);
		tanBeta = glm::max(sphericalRi.y, sphericalRo.y) / glm::abs(sphericalRi.y - sphericalRo.y);
	}
	else {
		sinAlpha = sinThetaI;
		tanBeta = 1.0f;
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


glm::vec3 PerfectSpecular::EvaluateBSDF(Shape * shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	return glm::vec3(1.0f);
}


glm::vec3 PerfectSpecular::SampleBSDF(glm::vec3 norm, glm::vec3 ri)
{
	glm::vec3 newDir = glm::reflect(ri, norm);

	// Compute based on Fre

	return glm::reflect(ri, norm);
}
