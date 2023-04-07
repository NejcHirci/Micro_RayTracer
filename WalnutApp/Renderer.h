#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <memory>
#include <glm/glm.hpp>


class Renderer
{
public:
	Renderer() = default;

	void Render(const Scene& scene, const Camera& camera);
	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetRenderImage() const { return m_RenderImage; }

private:
	glm::vec4 TraceRay(const Scene& scene, const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_RenderImage;
	uint32_t* m_RenderImageData = nullptr;
};