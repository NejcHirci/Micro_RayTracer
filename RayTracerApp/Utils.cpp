#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "Walnut/Random.h"

#include "Utils.h"

glm::vec3 Utils::UniformSampleHemisphere()
{
	while (true)
	{
		float x = Walnut::Random::Float() * 2.0f - 1.0f;
		float y = Walnut::Random::Float() * 2.0f - 1.0f;
		float z = Walnut::Random::Float() * 2.0f - 1.0f;
		glm::vec3 pointInCube = { x, y, z };

		float sqrDstFromCentre = glm::dot(pointInCube, pointInCube);

		if (sqrDstFromCentre <= 1.0f)
		{
			return pointInCube / sqrDstFromCentre;
		}
	}
}

glm::vec3 Utils::CosineSampleHemisphere(glm::vec3 norm)
{
	auto u = Walnut::Random::Float();
	auto v = Walnut::Random::Float();
	float r = glm::sqrt(u);
	float theta = 2.0f * glm::pi<float>() * v;

	float x = r * glm::cos(theta);
	float z = r * glm::sin(theta);
	float y = glm::sqrt(glm::max(0.0f, 1.0f - x * x - z * z));

	return glm::vec3{ x, y, z };
}


glm::vec3 Utils::UniformSampleSphere(float r)
{
	float u = Walnut::Random::Float();
	float v = Walnut::Random::Float();

	// Convert to spherical coordinates
	float theta = 2 * glm::pi<float>() * u;
	float phi = glm::acos(2 * v - 1);

	// Convert to Cartesian coordinates
	return glm::vec3{
		r * glm::sin(phi) * glm::cos(theta),
		r * glm::sin(phi) * glm::sin(theta),
		r * glm::cos(phi)
	};
}


glm::vec3 Utils::CartesianToSpherical(glm::vec3 ro)
{	
	// r, theta, phi
	return glm::vec3{
		1.0f,
		glm::acos(ro.y),
		std::atan2f(ro.z, ro.x)
	};
}


uint32_t Utils::ConvertToRGBA(const glm::vec4& color)
{
	// Use gamma correction 2.2
	glm::vec4 col = glm::pow(color, glm::vec4(1.0f / 2.2f));
	
	uint8_t r = (uint8_t)(glm::sqrt(col.r) * 255.0f);
	uint8_t g = (uint8_t)(glm::sqrt(col.g) * 255.0f);
	uint8_t b = (uint8_t)(glm::sqrt(col.b) * 255.0f);
	uint8_t a = (uint8_t)(col.a * 255.0f);

	uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
	return result;
}

void Utils::RotateX(const glm::mat4& m, float ang)
{
	glm::rotate(m, ang, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Utils::RotateY(const glm::mat4& m, float ang)
{
	glm::rotate(m, ang, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Utils::RotateZ(const glm::mat4& m, float ang)
{
	glm::rotate(m, ang, glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::mat4 Utils::BuildRotateTranslate(glm::vec3 rotAng, glm::vec3 translate)
{
	glm::mat4 mat {
		glm::vec4{1.0f, 0.0f, 0.0f, 0.0f},
		glm::vec4{0.0f, 1.0f, 0.0f, 0.0f},
		glm::vec4{0.0f, 0.0f, 1.0f, 0.0f},
		glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}
	};

	mat = glm::translate(mat, translate);
	mat = glm::rotate(mat, glm::radians(rotAng.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mat = glm::rotate(mat, glm::radians(rotAng.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, glm::radians(rotAng.x), glm::vec3(1.0f, 0.0f, 0.0f));
	

	return mat;
}

void Utils::Translate(const glm::mat4& m, glm::vec3 translate)
{
	glm::translate(m, translate);
}

glm::vec3 Utils::ApplyPoint(const glm::mat4& m, glm::vec3 p)
{
	glm::vec4 result = m * glm::vec4(p, 1.0f);
	return glm::vec3(result.x / result.w, result.y / result.w, result.z / result.w);
}

glm::vec3 Utils::ApplyVector(const glm::mat4& m, glm::vec3 v)
{
	return glm::vec3(m * glm::vec4(v, 0.0f));
}

glm::vec3 Utils::ApplyNormal(const glm::mat4& m, glm::vec3 n)
{
	glm::mat4 inv = glm::inverse(m);
	return glm::normalize(glm::vec3(inv * glm::vec4(n, 0.0f)));
}



