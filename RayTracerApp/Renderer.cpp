#include <iostream>

#include "Walnut/Random.h"

#include "Renderer.h"


// TODO: Implement a true path tracing renderer


namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{		
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}


void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_RenderImage->GetWidth() * m_RenderImage->GetHeight() * sizeof(glm::vec4));

	// Image Data Filling Loop
	for (uint32_t y = 0; y < m_RenderImage->GetHeight(); y++) 
	{
		for (uint32_t x = 0; x < m_RenderImage->GetWidth(); x++)
		{
			glm::vec4 color = PerPixel(x, y);
			m_AccumulationData[y * m_RenderImage->GetWidth() + x] += color;

			glm::vec4 accumulatedColor = m_AccumulationData[y * m_RenderImage->GetWidth() + x];
			accumulatedColor = accumulatedColor / (float)m_FrameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
			m_RenderImageData[y * m_RenderImage->GetWidth() + x] = Utils::ConvertToRGBA(accumulatedColor);
		}
	}
	m_RenderImage->SetData(m_RenderImageData);

	// Accumulation
	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_RenderImage) {
		// No resize needed
		if (m_RenderImage->GetWidth() == width && m_RenderImage->GetHeight() == height)
			return;

		m_RenderImage->Resize(width, height);
	}
	else {
		m_RenderImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_RenderImageData;
	m_RenderImageData = new uint32_t[width * height];	
	
	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	// In Vulkan, there is gl_launchID which is the id of the current pixel

	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_RenderImage->GetWidth()];


	//float u = ((float)x + Walnut::Random::Float()) / (float)m_RenderImage->GetWidth();
	//float v = ((float)y + Walnut::Random::Float()) / (float)m_RenderImage->GetHeight();
	//glm::vec2 coord = { u, v };
	//coord = coord * 2.0f - 1.0f; // -1 -> 1
	//glm::vec4 target = m_ActiveCamera->GetInverseProjection() * glm::vec4(coord.x, coord.y, 1, 1);
	//ray.Direction = glm::vec3(m_ActiveCamera->GetInverseView() * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space

	int bounces = 20;

	glm::vec3 radiance(0.0f);
	glm::vec3 factor(1.0f);

	for (int bounce = 0; bounce < bounces; bounce++)
	{
		Renderer::HitPayLoad payload = TraceRay(ray);


		// Miss
		if (payload.HitDistance < 0)
		{	
			break;
			auto unit_dir = glm::normalize(ray.Direction);
			float t = 0.5 * (unit_dir.y + 1.0);
			glm::vec3 skyColor = (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
			radiance += factor * skyColor;
			break;
		}

		glm::vec3 wo = -ray.Direction;

		// Hit the light source
		if (payload.LightHit)
		{
			// Direct light hit
			if (bounce == 0) {
				radiance = factor * sampleLight();
			}
			break;
		}

		// sample Light from the light source
		glm::vec3 Ld = sampleLight();
		radiance += factor * Ld;

		// Randomly sample a ray from intersection
		glm::vec3 wi = sampleHemisphere();
		glm::vec3 brdf = sampleBSDF(payload, wi, wo);

		float cosTheta = glm::dot(wi, payload.WorldNormal);
		factor *= brdf * glm::abs(cosTheta);

		// Bounce
		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f; // Offset the origin a bit to avoid self intersection
		ray.Direction = wi;

		// Russian Roulette
		if (bounce > 3) {
			float p = glm::max(factor.x, glm::max(factor.y, factor.z));
			if (Walnut::Random::Float() > p)
				break;
			factor /= p;
		}
	}

	return glm::vec4(radiance, 1.0f);
}


// This is a CPU implementation of a shader with RayGen equivalent in Vulkan
Renderer::HitPayLoad Renderer::TraceRay(const Ray& ray)
{
	int closestShapeID = -1;
	float hitDistance = FLT_MAX;
	bool lightHit = false;


	// First we need to find the closest shape that intersects with the ray
	for (size_t i=0; i < m_ActiveScene->shapes.size(); i++) 
	{
		Shape* shape = m_ActiveScene->shapes[i];
		float distance = shape->Intersect(ray);

		if (distance < 0.0f)
			continue;

		if (distance > 0.0f && distance < hitDistance) {
			hitDistance = distance;
			closestShapeID = (int)i;
		}
	}

	// Check if we hit a light source
	for (size_t j = 0; j < m_ActiveScene->lights.size(); j++)
	{
		Light light = m_ActiveScene->lights[j];
		float distance = light.shape->Intersect(ray);
		if (distance < 0.0f)
			continue;
		if (distance > 0.0f && distance < hitDistance) {
			hitDistance = distance;
			closestShapeID = (int)j;
			lightHit = true;
		}
	}

	if (closestShapeID < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestShapeID, lightHit);
}


Renderer::HitPayLoad Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayLoad payload;
	payload.HitDistance = -1.0f;
	return payload;
}


Renderer::HitPayLoad Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex, bool lightHit)
{
	Renderer::HitPayLoad payload;
	payload.ObjectIndex = objectIndex;
	payload.HitDistance = hitDistance;

	Shape* closestShape;
	if (lightHit) 
	{
		payload.LightHit = true;
		closestShape = m_ActiveScene->lights[objectIndex].shape;
	} 
	else 
	{
		payload.LightHit = false;
		closestShape = m_ActiveScene->shapes[objectIndex];
	}

	glm::vec3 origin = ray.Origin - closestShape->Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestShape->Position;

	return payload;
}


glm::vec3 Renderer::sampleBSDF(Renderer::HitPayLoad payload, glm::vec3 inboundRay, glm::vec3 outboundRay) {
	// Lambert

	const Shape* shape = m_ActiveScene->shapes[payload.ObjectIndex];
	glm::vec3 shapeColor = m_ActiveScene->materials[shape->MaterialIndex].Albedo;

	return shapeColor / glm::pi<float>();
}


glm::vec3 Renderer::sampleLight()
{

	return m_ActiveScene->lights[0].Color;
}


glm::vec3 Renderer::sampleHemisphere() {

	float theta = glm::acos(Walnut::Random::Float());
	float phi = Walnut::Random::Float() * 2.0f * glm::pi<float>();

	// Convert to cartesian
	glm::vec3 direction {
		glm::sin(theta) * glm::cos(phi),
		glm::sin(theta) * glm::sin(phi),
		glm::cos(theta)
	};

	return direction;
}
