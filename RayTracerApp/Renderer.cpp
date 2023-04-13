#include <iostream>
#include <execution>

#include "Walnut/Random.h"

#include "Renderer.h"
#include "Utils.h"


void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_RenderImage->GetWidth() * m_RenderImage->GetHeight() * sizeof(glm::vec4));

	std::for_each(std::execution::par, m_VerticalIter.begin(), m_VerticalIter.end(),
		[this](uint32_t y) 
		{
			// Lambda function for parallel execution
			for (uint32_t x = 0; x < m_RenderImage->GetWidth(); x++)
			{
				glm::vec4 color = PerPixel(x, y);
				m_AccumulationData[y * m_RenderImage->GetWidth() + x] += color;

				glm::vec4 accumulatedColor = m_AccumulationData[y * m_RenderImage->GetWidth() + x];
				accumulatedColor = accumulatedColor / (float)m_FrameIndex;

				accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
				m_RenderImageData[y * m_RenderImage->GetWidth() + x] = Utils::ConvertToRGBA(accumulatedColor);
			}
		});

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

	m_HorizontalIter.resize(width);
	m_VerticalIter.resize(height);
	for(uint32_t i=0; i < width; i++)
		m_HorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_VerticalIter[i] = i;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	// In Vulkan, there is gl_launchID which is the id of the current pixel

	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	//ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_RenderImage->GetWidth() + x];


	float u = ((float)x + Walnut::Random::Float()) / (float)m_RenderImage->GetWidth();
	float v = ((float)y + Walnut::Random::Float()) / (float)m_RenderImage->GetHeight();
	glm::vec2 coord = { u, v };
	coord = coord * 2.0f - 1.0f; // -1 -> 1
	glm::vec4 target = m_ActiveCamera->GetInverseProjection() * glm::vec4(coord.x, coord.y, 1, 1);
	ray.Direction = glm::vec3(m_ActiveCamera->GetInverseView() * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
	ray.Direction = glm::normalize(ray.Direction);

	int bounces = 1000;

	glm::vec3 radiance(0.0f);
	glm::vec3 rayColor(1.0f);

	for (int bounce = 0; bounce < bounces; bounce++)
	{
		Renderer::HitPayLoad payload = TraceRay(ray);

		if (payload.HitDistance < 0) {

			glm::vec3 skyColor = glm::vec3(0.5f, 0.7f, 1.0f);
			auto t = 0.5f * (ray.Direction.y + 1.0f);
			radiance = rayColor * (glm::mix(glm::vec3(1.0f), skyColor, t)) * 0.1f;
			break;
		}

		glm::vec3 rInbound = -ray.Direction;

		ray.Origin = payload.WorldPosition;
		ray.Direction = Utils::UniformSampleHemisphere();
		ray.Direction *= glm::sign(glm::dot(payload.WorldNormal, ray.Direction));

		if (payload.LightHit) 
		{
			glm::vec3 emmittedLight = m_ActiveScene->Lights[0]->SamplePoint();
			radiance += rayColor * emmittedLight;

			break;
		}

		rayColor *= EvaluateBSDF(payload, rInbound, ray.Direction);
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
	for (size_t i=0; i < m_ActiveScene->Shapes.size(); i++)
	{
		Shape* shape = m_ActiveScene->Shapes[i];
		float distance = shape->Intersect(ray);

		if (distance < 0.0f)
			continue;

		if (distance > 0.0f && distance < hitDistance) {
			hitDistance = distance;
			closestShapeID = (int)i;
		}
	}

	// Check if we hit a light source
	for (size_t j = 0; j < m_ActiveScene->Lights.size(); j++)
	{
		Light* light = m_ActiveScene->Lights[j];
		Shape* shape = light->Shape;
		float distance = light->Shape->Intersect(ray);
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
		closestShape = m_ActiveScene->Lights[objectIndex]->Shape;
	} 
	else 
	{
		payload.LightHit = false;
		closestShape = m_ActiveScene->Shapes[objectIndex];
	}

	glm::vec3 origin = ray.Origin - closestShape->Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestShape->Position;

	return payload;
}


glm::vec3 Renderer::EvaluateBSDF(Renderer::HitPayLoad payload, glm::vec3 inboundRay, glm::vec3 outboundRay) {
	// Lambert
	const Shape* shape = m_ActiveScene->Shapes[payload.ObjectIndex];
	Material* material = m_ActiveScene->Materials[shape->MaterialIndex];

	glm::vec3 color = material->EvaluateBSDF(payload.WorldNormal, inboundRay, outboundRay);
	return color;
}
