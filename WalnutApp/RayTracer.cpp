#include "Walnut/Application.h"

void Render() {
	// Graphics
	auto app = Walnut::Application::Get();
	VkDevice device = app.GetDevice();

	VkCommandBuffer commandBuffer = app.GetCommandBuffer();
	VkInstance instance = app.GetInstance();
	VkPhysicalDevice physicalDevice = app.GetPhysicalDevice();
	
}