#include <iostream>

#include "Walnut/Random.h"

#include "Renderer.h"


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
	Ray ray;
	ray.Origin = camera.GetPosition();

	// Image Data Filling Loop
	for (uint32_t y = 0; y < m_RenderImage->GetHeight(); y++) 
	{
		for (uint32_t x = 0; x < m_RenderImage->GetWidth(); x++)
		{
			ray.Direction = camera.GetRayDirections()[x + y * m_RenderImage->GetWidth()];

			glm::vec4 color = TraceRay(scene, ray);
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


// This is a CPU implementation of a shader
glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	// No objects in the scene
	if (scene.shapes.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Shape* closestShape = nullptr;
	float hitDistance = FLT_MAX;

	for (Shape* shape : scene.shapes) 
	{
		float distance = shape->Intersect(ray);

		if (distance > 0.0f && distance < hitDistance) {
			hitDistance = distance;
			closestShape = shape;
		}
	}
	
	glm::vec3 hitPoint = ray.Origin + ray.Direction * hitDistance;
	glm::vec3 normal = glm::normalize(hitPoint);

	// Lambert
	float diffuse = glm::max(glm::dot(normal, -scene.lights[0].Direction), 0.0f);


	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= diffuse;
	return glm::vec4(sphereColor * scene.lights[0].Color, 1.0f);
}
