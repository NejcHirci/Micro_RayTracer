#pragma once

#include <cmath>
#include <glm/glm.hpp>

struct Utils
{
	// Sampling functions

	static glm::vec3 UniformSampleHemisphere();

	static glm::vec3 CosineSampleHemisphere();

	// Utility functions

	static glm::vec3 CartesianToSpherical(glm::vec3 ro);

	static uint32_t ConvertToRGBA(const glm::vec4& color);

};
