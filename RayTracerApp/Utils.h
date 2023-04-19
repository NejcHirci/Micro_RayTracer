#pragma once

#include <cmath>
#include <glm/glm.hpp>

struct Utils
{
	// Sampling functions

	static glm::vec3 UniformSampleHemisphere();

	static glm::vec3 CosineSampleHemisphere(glm::vec3 norm);

	static glm::vec3 UniformSampleSphere(float r);

	// Utility functions

	static glm::vec3 CartesianToSpherical(glm::vec3 ro);

	static uint32_t ConvertToRGBA(const glm::vec4& color);

	// Transformation functions 
	static void RotateX(const glm::mat4& m, float ang);

	static void RotateY(const glm::mat4& m, float ang);

	static void RotateZ(const glm::mat4& m, float ang);

	static glm::mat4 BuildRotateTranslate(glm::vec3 rotAng, glm::vec3 translate);

	static void Translate(const glm::mat4& m, glm::vec3 translate);

	// Apply transforms

	static glm::vec3 ApplyPoint(const glm::mat4& m, glm::vec3 p);

	static glm::vec3 ApplyVector(const glm::mat4& m, glm::vec3 v);

	static glm::vec3 ApplyNormal(const glm::mat4& m, glm::vec3 n);

};
