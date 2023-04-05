#include "Renderer.h"

#include "Walnut/Random.h"


void Renderer::Render()
{
	// 
	// Image Data Filling Loop
	for (uint32_t y = 0; y < m_RenderImage->GetHeight(); y++) 
	{
		for (uint32_t x = 0; x < m_RenderImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_RenderImage->GetWidth(), (float)y / (float)m_RenderImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; // -1 -> 1
			m_RenderImageData[y * m_RenderImage->GetWidth() + x] = fragShader(coord);
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
uint32_t Renderer::fragShader(glm::vec2 coord)
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

	if (d >= 0.0f)
		return 0xffff00ff;

	return 0xff000000;
}
