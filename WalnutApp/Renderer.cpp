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

	// Image Data Filling Loop
	for (uint32_t y = 0; y < m_RenderImage->GetHeight(); y++) 
	{
		for (uint32_t x = 0; x < m_RenderImage->GetWidth(); x++)
		{
			glm::vec4 color = PerPixel(x, y);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_RenderImageData[y * m_RenderImage->GetWidth() + x] = Utils::ConvertToRGBA(color);
		}
	}
	m_RenderImage->SetData(m_RenderImageData);
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
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	// In Vulkan, there is gl_launchID which is the id of the current pixel
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_RenderImage->GetWidth()]; 

	glm::vec3 color(0.0f);

	int bounces = 2;
	float multiplier = 1.0f;

	// Perform multiple ray bounces
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayLoad payload = TraceRay(ray);
		if (payload.HitDistance < 0)
		{	
			glm::vec3 skyColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			color += skyColor * multiplier;
			break;
		}

		// Lambert
		float diffuse = glm::max(glm::dot(payload.WorldNormal, -(m_ActiveScene->lights[0].Direction)), 0.0f);

		const Shape* shape = m_ActiveScene->shapes[payload.ObjectIndex];
		glm::vec3 sphereColor = m_ActiveScene->materials[shape->MaterialIndex].Albedo;
		sphereColor *= diffuse;
		color += sphereColor * multiplier;

		multiplier *= 0.7f;


		// Reflection
		ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f; // Offset the origin a bit to avoid self intersection
		ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal);
	}


	return glm::vec4(color, 1.0f);
}


// This is a CPU implementation of a shader with RayGen equivalent in Vulkan
Renderer::HitPayLoad Renderer::TraceRay(const Ray& ray)
{
	int closestShapeID = -1;
	float hitDistance = FLT_MAX;

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

	if (closestShapeID < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestShapeID);
}


Renderer::HitPayLoad Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayLoad payload;
	payload.HitDistance = -1.0f;
	return payload;
}


Renderer::HitPayLoad Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayLoad payload;
	payload.ObjectIndex = objectIndex;
	payload.HitDistance = hitDistance;


	const Shape* closestShape = m_ActiveScene->shapes[objectIndex];

	glm::vec3 origin = ray.Origin - closestShape->Position;
	payload.WorldPosition = origin + ray.Direction * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestShape->Position;

	return payload;
}
