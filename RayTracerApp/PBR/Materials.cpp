#include <glm/gtc/constants.hpp>

#include "../Utils.h"
#include "Materials.h"

glm::vec3 Lambert::EvaluateBSDF(Shape* shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	glm::vec3 sphericalRo = Utils::CartesianToSpherical(ro);
	glm::vec3 sphericalRi = Utils::CartesianToSpherical(ri);

	return Albedo * glm::one_over_pi<float>();
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

// ri must be in local space
glm::vec3 Lambert::SampleBSDF(glm::vec3 norm, glm::vec3 ri)
{
	glm::vec3 v = Utils::CosineSampleHemisphere(norm);

	return v;
}


// ri must be in local space
glm::vec3 OrenNayar::SampleBSDF(glm::vec3 norm, glm::vec3 ri) 
{
	glm::vec3 v = Utils::CosineSampleHemisphere(norm);

	return v;
}

float Material::CalculatePdf(glm::vec3 wo, glm::vec3 wi)
{	
	return glm::dot(wo, wi) > 0 ? glm::one_over_pi<float>() : 0.0f;
}


glm::vec3 PerfectSpecular::EvaluateBSDF(Shape * shape, glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	// Use Schlick-Fresnel approximation
	auto f1 = fresnel1;
	auto f2 = fresnel2;
	
	// Perfect Mirror
	if (f1 == 0 && f2 == 0) {
		return glm::vec3(1.0f);
	}

	float cosThetaI = glm::clamp(glm::dot(ro, ri), -1.0f, 1.0f);

	bool entering = cosThetaI > 0.0f;
	if (!entering) 
	{
		auto temp = f1;
		f1 = f2;
		f2 = temp;
		cosThetaI = glm::abs(cosThetaI);
	}

	auto sinThetaI = glm::sqrt(glm::max(0.0f, 1.0f - cosThetaI * cosThetaI));
	auto sinThetaT = f1 / f2 * sinThetaI;

	// Total internal reflection
	if (sinThetaT >= 1.0f) return glm::vec3(1.0f);

	auto cosThetaT = glm::sqrt(glm::max(0.0f, 1.0f - sinThetaT * sinThetaT));
	auto Rparl = ((f2 * cosThetaI) - (f1 * cosThetaT)) / ((f2 * cosThetaI) + (f1 * cosThetaT));
	auto Rperp = ((f1 * cosThetaI) - (f2 * cosThetaT)) / ((f1 * cosThetaI) + (f2 * cosThetaT));

	return glm::vec3((Rparl * Rparl + Rperp * Rperp) / 2.0f);
}

glm::vec3 PerfectSpecular::SampleBSDF(glm::vec3 norm, glm::vec3 ri)
{
	return glm::reflect(ri, norm);
}
