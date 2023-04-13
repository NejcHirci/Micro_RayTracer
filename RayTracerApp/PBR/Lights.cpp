#include "Lights.h"

glm::vec3 DiffuseAreaLight::SamplePoint() 
{
	return Color * Intensity;
	
}

glm::vec3 DiffuseAreaLight::SampleLightRay(glm::vec3 point)
{
	return glm::vec3(0.0f);

}


// Evaluate the light color at a point
glm::vec3 DiffuseAreaLight::EvaluateLight(glm::vec3 point)
{
	return glm::vec3(0.0f);

}