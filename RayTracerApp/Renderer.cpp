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
	ray.Direction = m_ActiveCamera->GetRayDirections()[y * m_RenderImage->GetWidth() + x];

	int bounces = 20;

	glm::vec3 radiance(0.0f);
	glm::vec3 rayColor(1.0f);

	glm::vec3 newDir;
	glm::vec3 f;
	float pdf;

	for (int bounce = 0; bounce < bounces; bounce++)
	{
		// Step one we should intersect ray with scene
		Renderer::HitPayLoad payload = TraceRay(ray);

		if (payload.HitDistance <= 0) 
		{
			// Skybox color based on y direction
			//float t = 0.5f * (ray.Direction.y + 1.0f);
			//radiance += rayColor * ( t * glm::vec3(0.5f, 0.7f, 1.0f) + (1-t) * glm::vec3(1.0f));
			break; 
		}

		if (bounce == 0) {
			if (payload.LightHit) {
				radiance += rayColor * m_ActiveScene->Lights[payload.ObjectIndex]->LightEmission(payload.WorldNormal, -ray.Direction);
				break;
			}
		}
		
		// Add direct illumination
		radiance += rayColor * EvaluateDirectIllumination(payload);

		Shape* shape = m_ActiveScene->Shapes[payload.ObjectIndex];
		Material* material = m_ActiveScene->Materials[shape->MaterialIndex];

		newDir = material->SampleBSDF(payload.WorldNormal, -ray.Direction);

		// Only if same hemisphere
		f = material->EvaluateBSDF(shape, payload.WorldNormal, -ray.Direction, newDir);
		pdf = material->CalculatePdf(-ray.Direction, newDir);

		rayColor *= f;

		ray.Origin = payload.WorldPosition;
		ray.Direction = newDir;

		// Russian roulette
		if (bounce > 3) {
			float p = glm::max(0.01f, 1.0f - glm::max(rayColor.x, glm::max(rayColor.y, rayColor.z)));
			if (Walnut::Random::Float() < p)
				break;
			rayColor /= 1.0f - p;
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

	payload.WorldPosition = ray.Origin + ray.Direction * hitDistance;
	payload.WorldNormal = closestShape->GetNormal(payload.WorldPosition);

	return payload;
}


glm::vec3 Renderer::EvaluateDirectIllumination(Renderer::HitPayLoad payload)
{
	// Add direct illumination from lights for path reuse
	Light* light = m_ActiveScene->Lights[0];
	Ray shadowRay = light->SampleLightRay(payload.WorldPosition);

	// Move the ray origin a bit to avoid self intersection
	shadowRay.Origin += payload.WorldNormal * 0.001f;

	// We already know which light we are hitting so we should just check for intersections with shapes
	// First we need to find the closest shape that intersects with the ray
	float distance;
	for (size_t i = 0; i < m_ActiveScene->Shapes.size(); i++) {
		Shape* shape = m_ActiveScene->Shapes[i];
		distance = shape->Intersect(shadowRay);
	}

	if (distance > 0.0f) return glm::vec3(0.0f);

	// Get Intersection
	distance = light->Shape->Intersect(shadowRay);

	// Compute intersection normal
	glm::vec3 lightIntersectionPos = shadowRay.Origin - shadowRay.Direction * distance;


	glm::vec3 lightNormal = light->Shape->GetNormal(lightIntersectionPos);

	// Compute the pdf of the light
	float pdf = light->CalculatePdf(payload.WorldNormal, payload.WorldPosition, lightNormal, lightIntersectionPos);

	if (pdf <= 0.0f) return glm::vec3(0.0f);

	return light->LightEmission(lightNormal, -shadowRay.Direction) / (pdf);
}


glm::vec3 Renderer::SampleDirection(Renderer::HitPayLoad payload, glm::vec3 ro)
{
	Shape* shape = m_ActiveScene->Shapes[payload.ObjectIndex];
	Material* material = m_ActiveScene->Materials[shape->MaterialIndex];

	// Sample the new local direction
	glm::vec3 newDir = material->SampleBSDF(payload.WorldNormal, ro);

	return newDir;
}
