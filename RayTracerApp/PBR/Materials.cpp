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

	float a = 1.0f - 0.5f * (Roughness * Roughness) / (Roughness * Roughness + 0.33f);
	float b = 0.45f * (Roughness * Roughness) / (Roughness * Roughness + 0.09f);

	float alpha = glm::max(sphericalRo.y, sphericalRi.y);
	float beta = glm::min(sphericalRo.y, sphericalRi.y);

	float factor = glm::max(0.0f, glm::cos(sphericalRi.z - sphericalRo.z));

	float pdf = CalculatePdf(ro, ri);

	return Albedo * (a + b * factor * glm::sin(alpha) * glm::tan(beta)) * glm::one_over_pi<float>();
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
	return glm::reflect(ri, norm);
}
