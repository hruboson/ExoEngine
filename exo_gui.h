#pragma once

#include "device.h"
#include "swapchain.h"
#include "frame_info.h"

#include "exo_db.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include <array>
#include <string>

namespace exo {

	class ExoGui {
	public:
		ExoGui(ExoWindow &window, ExoDevice &device, VkRenderPass renderPass, uint32_t imageCount, ExoDB db);
		~ExoGui(); // to-do: cleanup

		ExoGui(const ExoGui&) = delete;
		ExoGui& operator = (const ExoGui&) = delete;

		ImGui_ImplVulkanH_Window mainWindowData;
		ImGui_ImplVulkanH_Window* wd;

		bool recreateSwapChain;

		// rendering
		ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		void newFrame();
		void runGui(FrameInfo frameInfo);
		void renderGui(VkCommandBuffer imGuiCommandBuffer);

		// components
		bool debug = false;
		std::vector<bool> windowsOpened;

		// chars for compatibility with cz
		const std::vector<int> specialCharCodes = {
			0x011B, 0x00ED, 0x008A, 0x0159, 0x0161, 0x017E, 0x017D, 0x0165, 0x010D, 0x010C, 0x010F, 0x016F, 0x00FD, 0x0148, 0xD83D, 0xDC41
		};

	private:
		ExoDevice& device;
		ExoWindow& window;
		ExoDB db;

		// setup
		uint32_t imageCount;
		VkRenderPass renderPass;

		ImGui_ImplVulkan_InitInfo getImGuiInitInfo();

		VkDescriptorPool imGuiDescriptorPool;
		void createImGuiDescriptorPool();

		VkCommandPool imGuiCommandPool;
		void createImGuiCommandPool();

		// SQLITE DB
		void getDbData();

		std::vector<std::vector<std::pair<std::string, std::string>>> planetData;
		void initWindowState();

		// helper variables
		const static std::string identifier;

	};

}