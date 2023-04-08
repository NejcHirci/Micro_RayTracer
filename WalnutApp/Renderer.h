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

	struct HitPayLoad
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		uint32_t ObjectIndex;
	};

	glm::vec4 PerPixel(); // RayGenShader in Vulkan


	HitPayLoad TraceRay(const Ray& ray);
	HitPayLoad ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex);
	HitPayLoad Miss(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image> m_RenderImage;

	const Scene *m_ActiveScene = nullptr;
	const Camera *m_ActiveCamera = nullptr;

	uint32_t* m_RenderImageData = nullptr;
};