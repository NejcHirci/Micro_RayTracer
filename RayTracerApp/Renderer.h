#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "Scene.h"

#include <memory>
#include <glm/glm.hpp>


// TODO:  Support for light sampling with specular materials.


class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};

	Renderer() = default;

	void Render(const Scene& scene, const Camera& camera);
	void OnResize(uint32_t width, uint32_t height);

	std::shared_ptr<Walnut::Image> GetRenderImage() const { return m_RenderImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }
	int GetFrameIndex() const { return m_FrameIndex; }

private:

	struct HitPayLoad
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		int ObjectIndex;
		bool LightHit;
	};

	glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGenShader in Vulkan


	HitPayLoad TraceRay(const Ray& ray);
	HitPayLoad ClosestHit(const Ray& ray, float hitDistance, int objectIndex, bool lightHit);
	HitPayLoad Miss(const Ray& ray);
	glm::vec3 Renderer::EvaluateDirectIllumination(Renderer::HitPayLoad payload);
	glm::vec3 Renderer::SampleDirection(Renderer::HitPayLoad payload, glm::vec3 ro);

private:
	Settings m_Settings;
	std::shared_ptr<Walnut::Image> m_RenderImage;

	std::vector<uint32_t> m_HorizontalIter, m_VerticalIter;

	const Scene *m_ActiveScene = nullptr;
	const Camera *m_ActiveCamera = nullptr;

	uint32_t* m_RenderImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
};