#include <glm/gtc/constants.hpp>

#include "../Utils.h"
#include "Materials.h"

glm::vec3 Lambert::EvaluateBSDF(glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	float cosTheta = glm::dot(worldNormal, ro);
	return Albedo * cosTheta;
}

glm::vec3 Lambert::SampleBSDF(glm::vec3 ro)
{
	return glm::vec3(1.0f);
}

glm::vec3 OrenNayar::EvaluateBSDF(glm::vec3 worldNormal, glm::vec3 ro, glm::vec3 ri)
{
	glm::vec3 sphericalRo = Utils::CartesianToSpherical(ro);
	glm::vec3 sphericalRi = Utils::CartesianToSpherical(ri);

	float a = 1.0f - 0.5f * (Roughness * Roughness) / (Roughness * Roughness + 0.33f);
	float b = 0.45f * (Roughness * Roughness) / (Roughness * Roughness + 0.09f);

	float alpha = glm::max(sphericalRo.y, sphericalRi.y);
	float beta = glm::min(sphericalRo.y, sphericalRi.y);

	float factor = glm::max(0.0f, glm::cos(sphericalRi.z - sphericalRo.z));

	return Albedo * (1.0f / glm::pi<float>()) * (a + b * factor * glm::sin(alpha) * glm::tan(beta));
}

glm::vec3 OrenNayar::SampleBSDF(glm::vec3 ro)
{
	return glm::vec3();
}
