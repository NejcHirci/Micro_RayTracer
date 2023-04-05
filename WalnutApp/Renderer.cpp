#include "Renderer.h"

#include "Walnut/Random.h"

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


void Renderer::Render()
{
	// Image Data Filling Loop
	for (uint32_t y = 0; y < m_RenderImage->GetHeight(); y++) 
	{
		for (uint32_t x = 0; x < m_RenderImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_RenderImage->GetWidth(), (float)y / (float)m_RenderImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // -1 -> 1
			glm::vec4 color = fragShader(coord);
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
glm::vec4 Renderer::fragShader(glm::vec2 coord)
{
	// Sphere
	float sphereRadius = 0.5f;

	// Ray
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);

	// Intersection
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - sphereRadius * sphereRadius;

	// Discriminant
	float d = b * b - 4.0f * a * c;

	if (d <= 0.0f) {
		return glm::vec4(0, 0, 0, 1);
	}

	// Closest intersection
	float t0 = (-b - glm::sqrt(d)) / (2.0f * a);
	
	glm::vec3 hitPoint = rayOrigin + rayDirection * t0;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 sphereColor = normal;
	return glm::vec4(normal, 1.0f);
}
