#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>


class Renderer
{
public:
	Renderer() = default;

	void Render();
	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetRenderImage() const { return m_RenderImage; }

private:
	glm::vec4 fragShader(glm::vec2 coord);
private:
	std::shared_ptr<Walnut::Image> m_RenderImage;
	uint32_t* m_RenderImageData = nullptr;
};