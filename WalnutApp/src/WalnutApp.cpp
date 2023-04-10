#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{
		// Initialize test scene

		// Materials
		Material pink_diffuse;
		pink_diffuse.Albedo = { 1.0f, 0.0f, 1.0f };
		m_Scene.materials.push_back(pink_diffuse);

		// Shape 1
		Sphere* sphere1 = new Sphere();
		sphere1->Position = { 0.0f, 0.0f, 0.0f };
		sphere1->Radius = 0.5f;
		sphere1->MaterialIndex = 0;
		m_Scene.shapes.push_back(sphere1);

		// Shape 2
		Sphere* sphere2 = new Sphere();
		sphere2->Position = { 0.0f, 1.0f, 0.0f };
		sphere2->Radius = 0.6f;
		sphere2->MaterialIndex = 0;
		m_Scene.shapes.push_back(sphere2);


		// Lights
		Light light;
		light.Position = { 0.0f, 0.0f, 0.0f };
		light.Direction = { 0.0f, 0.0f, -1.0f };
		light.Color = { 1.0f, 1.0f, 1.0f };
		m_Scene.lights.push_back(light);
	
	}

	virtual void OnUpdate(float ts) override
	{
		m_Camera.OnUpdate(ts);

	}

	virtual void OnUIRender() override
	{
		// Settings
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::End();

		// Scene 
		ImGui::Begin("Scene");
		// If at least one light in scene
		if (m_Scene.lights.size() > 0)
		{
			ImGui::SliderFloat3("Direction:", (float*)&m_Scene.lights[0].Direction, -1.0f, 1.0f);
			ImGui::SliderFloat("Intensity:", &m_Scene.lights[0].Intensity, 0.0f, 10.0f);
			ImGui::ColorEdit3("Color", &m_Scene.lights[0].Color.x);
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		
		auto image = m_Renderer.GetRenderImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;

	glm::vec3 lightDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec4 lightColor;

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}