#pragma once

#include "device.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace exo {

	class ExoGui {
	public:
		ExoGui(ExoWindow &window, ExoDevice &device);
		
		ExoGui(const ExoGui&) = delete;
		ExoGui& operator = (const ExoGui&) = delete;

		ImGui_ImplVulkanH_Window* wd;

	private:
		ExoDevice& device;
		ExoWindow& window;

		ImGui_ImplVulkan_InitInfo getImGuiInitInfo();

		VkRenderPass imGuiRenderPass;
		void createImGuiRenderPass();

		VkDescriptorPool imGuiDescriptorPool;
		void createImGuiDescriptorPool();
	};

}