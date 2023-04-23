#include <string>
#include <fstream>
#include <filesystem>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

using namespace Walnut;

class MainLayer : public Walnut::Layer
{
public:
	MainLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{
		// Initialize test scene
		m_Scene.CreateScene();
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts)) {
			m_Renderer.ResetFrameIndex();
		}

	}

	virtual void OnUIRender() override
	{
		// Settings
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("FPS: %d", (int)(1000.0f / m_LastRenderTime));
		ImGui::Text("Sample count: %d", m_Renderer.GetFrameIndex());

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Save"))
			SaveImage();

		if (ImGui::Button("Render"))
			Render();

		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrameIndex();

		ImGui::End();

		// Scene 
		ImGui::Begin("Scene");
		// If at least one light in scene
		if (m_Scene.Lights.size() > 0)
		{
			ImGui::SliderFloat3("Position:", (float*)&m_Scene.Lights[0]->Shape->Position, -5.0f, 5.0f);
			ImGui::SliderFloat("Intensity:", &m_Scene.Lights[0]->Intensity, 0.0f, 1000.0f);
			ImGui::ColorEdit3("Color", (float *)&m_Scene.Lights[0]->Color);
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

	void SaveImage()
	{
		auto image = m_Renderer.GetRenderImage();
		if (image)
		{
			// Save image as bmp
			auto as_bmp = [](const std::string& name) -> std::string {
				if (!(name.length() >= 4) && name.substr(name.length() - 4) == ".bmp")
					return name + ".bmp";
				else
					return name;
			};

			int w = image->GetWidth(), h = image->GetHeight();
			unsigned char* imgData = (unsigned char*)malloc(3 * w * h);
			memset(imgData, 0, 3 * w * h);

			uint32_t* rendererImageData = m_Renderer.GetRenderImageData();

			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++) {
					uint32_t color = rendererImageData[(h - 1 - y) * w + x];
					uint8_t b = (color & 0x000000ff);
					uint8_t g = (color & 0x0000ff00) >> 8;
					uint8_t r = (color & 0x00ff0000) >> 16;
					r = std::min(r, (uint8_t)255);
					g = std::min(g, (uint8_t)255);
					b = std::min(b, (uint8_t)255);

					int index = (x + y * w) * 3;
					imgData[index + 0] = (unsigned char)(r);
					imgData[index + 1] = (unsigned char)(g);
					imgData[index + 2] = (unsigned char)(b);
				}
			}



			FILE* f;
			int filesize = 54 + 3 * w * h;

			unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0 };
			unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
			unsigned char bmppad[3] = { 0,0,0 };

			bmpfileheader[2] = (unsigned char)(filesize);
			bmpfileheader[3] = (unsigned char)(filesize >> 8);
			bmpfileheader[4] = (unsigned char)(filesize >> 16);
			bmpfileheader[5] = (unsigned char)(filesize >> 24);

			bmpinfoheader[4] = (unsigned char)(w);
			bmpinfoheader[5] = (unsigned char)(w >> 8);
			bmpinfoheader[6] = (unsigned char)(w >> 16);
			bmpinfoheader[7] = (unsigned char)(w >> 24);
			bmpinfoheader[8] = (unsigned char)(h);
			bmpinfoheader[9] = (unsigned char)(h >> 8);
			bmpinfoheader[10] = (unsigned char)(h >> 16);
			bmpinfoheader[11] = (unsigned char)(h >> 24);

			f = fopen(as_bmp("render.bmp").c_str(), "wb");
			fwrite(bmpfileheader, 1, 14, f);
			fwrite(bmpinfoheader, 1, 40, f);
			for (int i = 0; i < h; i++)
			{
				fwrite(imgData+(w * (h - i - 1) * 3), 3, w, f);
				fwrite(bmppad, 1, (4 - (w * 3) % 4) % 4, f);
			}
			free(imgData);
			fclose(f);

			auto p = std::filesystem::path("./render.bmp");
			std::cout << "File saved at " << std::filesystem::weakly_canonical(p) << std::endl;
		}
	}

private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<MainLayer>();
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