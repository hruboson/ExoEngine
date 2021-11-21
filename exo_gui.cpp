#include "exo_gui.h"

#include <stdexcept>

namespace exo {

	ExoGui::ExoGui(ExoWindow& window, ExoDevice& device, VkRenderPass renderPass, uint32_t imageCount) : device{ device }, window{ window }, renderPass{ renderPass }, imageCount{ imageCount } {
		createImGuiDescriptorPool();
		createImGuiCommandPool();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); 
		(void)io;
		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		for(auto specialCharCode : specialCharCodes) {
			builder.AddChar(specialCharCode);
		}
		builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
		builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
		builder.BuildRanges(&ranges);
		io.Fonts->AddFontFromFileTTF("dearimgui/consola.ttf", 20, NULL, ranges.Data);

		ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = getImGuiInitInfo();
		ImGui_ImplVulkan_Init(&init_info, renderPass);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// one time command buffer can be done with helper funcitons on device class
		auto commandBuffer = device.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
		
	}

	ExoGui::~ExoGui() {
		vkDestroyDescriptorPool(device.device(), imGuiDescriptorPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	};

	ImGui_ImplVulkan_InitInfo ExoGui::getImGuiInitInfo() {
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = device.instance();
		init_info.PhysicalDevice = device.physicalDevice();
		init_info.Device = device.device();
		init_info.QueueFamily = device.graphicsQueueFamily();
		init_info.Queue = device.graphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = imGuiDescriptorPool;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.MinImageCount = ExoSwapChain::MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = imageCount;
		//init_info.CheckVkResultFn = check_vk_result;

		return init_info;
	}

	void ExoGui::createImGuiDescriptorPool() {
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &imGuiDescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("Couldn't create ImGui Descriptor pool!");
		}
	}

	void ExoGui::createImGuiCommandPool() {
		QueueFamilyIndices queueFamilyIndices = device.findPhysicalQueueFamilies();

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(device.device(), &commandPoolCreateInfo, nullptr, &imGuiCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("Could not create graphics command pool!");
		}
	}

	void ExoGui::newFrame() {
		// we tell imgui we are creating new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ExoGui::runGui() {
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Seznam planet");  // Begin must be close with End

			ImGui::Text(
				u8"P¯Ìliö ûluùouËk˝ k˘Ú ˙pÏl Ô·belskÈ Ûdy");  // Display some text (you can use a format strings too)

			if (ImGui::Button(u8"Slunce")) {
				
			}
			if (ImGui::Button(u8"Merkur")) {

			}
			if (ImGui::Button(u8"Venuöe")) {

			}
			if (ImGui::Button(u8"ZemÏ")) {

			}
			if (ImGui::Button(u8"Mars")) {

			}
			if (ImGui::Button(u8"Jupiter")) {

			}
			if (ImGui::Button(u8"Saturn")) {

			}
			if (ImGui::Button(u8"Uran")) {

			}
			if (ImGui::Button(u8"Neptun")) {

			}
			if (ImGui::Button(u8"Pluto")) {

			}

			ImGui::Checkbox("Debug", &debug);

			ImGui::End();
		}
		if (debug) {
			ImGui::Begin(
				"Debug",
				&debug);  // Pass a pointer to our bool variable (the window will have a
										// closing button that will clear the bool when clicked)
			ImGui::Text(
				"Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);

			if (ImGui::Button(u8"Zav¯Ìt")) debug = false;
			ImGui::End();
		}
	}

	void ExoGui::renderGui(VkCommandBuffer imGuiCommandBuffer) {
		// render the frame we created and occupied
		ImGui::Render();
		ImDrawData* drawdata = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawdata, imGuiCommandBuffer);
	}

}