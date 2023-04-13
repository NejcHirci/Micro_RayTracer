#include "Walnut/Random.h"

#include "Utils.h"

glm::vec3 Utils::UniformSampleHemisphere()
{
	while (true)
	{
		float x = Walnut::Random::Float() * 2.0f - 1.0f;
		float y = Walnut::Random::Float() * 2.0f - 1.0f;
		float z = Walnut::Random::Float() * 2.0f - 1.0f;
		glm::vec3 point = { x, y, z };
		float sqrDstFromCentre = glm::dot(point, point);

		if (sqrDstFromCentre <= 1.0f)
		{
			point /= glm::sqrt(sqrDstFromCentre);

			return point;
		}
	}
}

glm::vec3 Utils::CosineSampleHemisphere()
{
	return UniformSampleHemisphere();
}

glm::vec3 Utils::CartesianToSpherical(glm::vec3 ro)
{
	return glm::vec3{
		1.0f,
		std::atan2f(glm::sqrt(ro.x * ro.x + ro.y * ro.y), ro.z),
		std::atan2f(ro.y, ro.z)
	};
}

uint32_t Utils::ConvertToRGBA(const glm::vec4& color)
{
	uint8_t r = (uint8_t)(color.r * 255.0f);
	uint8_t g = (uint8_t)(color.g * 255.0f);
	uint8_t b = (uint8_t)(color.b * 255.0f);
	uint8_t a = (uint8_t)(color.a * 255.0f);

	uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
	return result;
}
