#include "exo_gui.h"
#include "IconsFontAwesome4.h"
#include "frame_info.h"

#include <stdexcept>
#include <utility>
#include <string>

#include <iostream>

namespace exo {
	const std::string ExoGui::btn_identifier = "##ID";

	ExoGui::ExoGui(ExoWindow& window, ExoDevice& device, VkRenderPass renderPass, uint32_t imageCount, ExoDB db) : device{ device }, window{ window }, renderPass{ renderPass }, imageCount{ imageCount }, db{ db } {
		createImGuiDescriptorPool();
		createImGuiCommandPool();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		// Special ranges and chars
		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		for (auto specialCharCode : specialCharCodes) {
			builder.AddChar(specialCharCode);
		}
		builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
		builder.BuildRanges(&ranges);
		io.Fonts->AddFontFromFileTTF("dearimgui/Mulish-Regular.ttf", 20, NULL, ranges.Data);

		// Icons
		ImFontConfig config;
		config.MergeMode = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromFileTTF("dearimgui/fontawesome-webfont.ttf", 20, &config, icon_ranges);

		ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = getImGuiInitInfo();
		ImGui_ImplVulkan_Init(&init_info, renderPass);

		// Setup Dear ImGui style
		setStyle();
		//ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// one time command buffer can be done with helper funcitons on device class
		auto commandBuffer = device.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();

		getDbData();
		initWindowState();
	}

	ExoGui::~ExoGui() {
		vkDestroyDescriptorPool(device.device(), imGuiDescriptorPool, nullptr);
		vkDestroyCommandPool(device.device(), imGuiCommandPool, nullptr);
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

	void ExoGui::setStyle() {

		// source: https://github.com/ocornut/imgui/issues/707#issuecomment-917151020
		// all rights to this theme belong to janekb04
		//ImVec4* colors = ImGui::GetStyle().Colors;
		//colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		//colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		//colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		//colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		//colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		//colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		//colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		//colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		//colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		//colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		//colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		//colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		//colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		//colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		//colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		//colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		//colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		//colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		//colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		//colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		//colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		//colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		//colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		//colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		//colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		//colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		//colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		//colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		//colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		//colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		//colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		//colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		////colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		////colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		//colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		//colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		//colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		//colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		//colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		//colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		//colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		//colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		//colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
		//
		//ImGuiStyle& style = ImGui::GetStyle();
		//style.WindowPadding = ImVec2(8.00f, 8.00f);
		//style.FramePadding = ImVec2(5.00f, 2.00f);
		//style.CellPadding = ImVec2(6.00f, 6.00f);
		//style.ItemSpacing = ImVec2(6.00f, 6.00f);
		//style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		//style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		//style.IndentSpacing = 25;
		//style.ScrollbarSize = 15;
		//style.GrabMinSize = 10;
		//style.WindowBorderSize = 1;
		//style.ChildBorderSize = 1;
		//style.PopupBorderSize = 1;
		//style.FrameBorderSize = 1;
		//style.TabBorderSize = 1;
		//style.WindowRounding = 7;
		//style.ChildRounding = 4;
		//style.FrameRounding = 3;
		//style.PopupRounding = 4;
		//style.ScrollbarRounding = 9;
		//style.GrabRounding = 3;
		//style.LogSliderDeadzone = 4;
		//style.TabRounding = 4;

		// source: https://github.com/ocornut/imgui/issues/707#issuecomment-678611331
		// all rights to this theme belong to OverShifted
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		style.GrabRounding = style.FrameRounding = 2.3f;
	}

	void ExoGui::newFrame() {
		// we tell imgui we are creating new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ExoGui::runGui(FrameInfo frameInfo) {
		{ // new scope
			ImGui::Begin(u8"Seznam těles", nullptr, ImGuiWindowFlags_NoMove);  // Begin must be closed with End
			int i = 0;

			auto lst_front = windowsOpened.begin();

			for (auto& row : this->planetData) {
				if (lst_front == windowsOpened.end()) {
				
				}
				else {
					int objId = i + 1; // because Sun is also object
					if (ImGui::Button(row.at(0).second.c_str())) {
						windowsOpened.insert(lst_front, true);
						std::cout << *lst_front << std::endl;
					}
					ImGui::SameLine();
					std::string buttonName = ICON_FA_SEARCH + ExoGui::btn_identifier + std::to_string(i); // each button must have unique identifier so adding ##_num_ makes it unique but doesn't render it
					if (ImGui::Button(buttonName.c_str())) {
						//windowsOpened.at(i) = true; // open window when zooming to planet
						timeSpeed = 0; // Stop time when zooming on planet
				
						frameInfo.viewerObject.transform.rotation = glm::vec3{ 0, 180.f, 0 };
						frameInfo.viewerObject.transform.translation = frameInfo.gameObjects.at(objId).transform.translation + glm::vec3{ frameInfo.gameObjects.at(objId).transform.scale.x * 5, 0, frameInfo.gameObjects.at(objId).transform.scale.x * 5 };
						frameInfo.camera.setViewYXZ(frameInfo.viewerObject.transform.translation, frameInfo.viewerObject.transform.rotation);
					}
					i++;
					++lst_front;
				};	
			}
			ImGui::End();
		}
		{
			int i = 0;
			
			auto lst_front = windowsOpened.begin();

			for (auto& row : this->planetData) {
				if (lst_front == windowsOpened.end()) {
					
				}
				else {
					if (*lst_front) {
						ImGui::Begin(row.at(0).second.c_str(), &(*lst_front), ImGuiWindowFlags_HorizontalScrollbar); // NO WAAAAAAAY IM GENIUS &(*lst_front)
						for (auto& data : row) {
								ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(112, 147, 184, 255));
								std::string text1 = data.first + " : ";
								std::string text2 = data.second;
								ImGui::TextWrapped(text1.c_str());
								ImGui::PopStyleColor();
								ImGui::SameLine();
								ImGui::TextWrapped(text2.c_str());
						}
						if (ImGui::Button(u8"Zavřít")) *lst_front = false;
						ImGui::SameLine();
						if (ImGui::Button(ICON_FA_SEARCH)) {
							int objId = i + 1; // because Sun is also object
							frameInfo.viewerObject.transform.rotation = glm::vec3{ 0, 180.f, 0 };
							frameInfo.viewerObject.transform.translation = frameInfo.gameObjects.at(objId).transform.translation + glm::vec3{ frameInfo.gameObjects.at(objId).transform.scale.x * 5, 0, frameInfo.gameObjects.at(objId).transform.scale.x * 5 };
							frameInfo.camera.setViewYXZ(frameInfo.viewerObject.transform.translation, frameInfo.viewerObject.transform.rotation);
						}
						ImGui::End();
					}
					++lst_front;
				}
				i++;
			}
		}
		{
			ImGui::Begin(u8"Scény", nullptr);
			if (ImGui::Button(u8"Zobrazit Sluneční soustavu ve skutečné velikosti")) {
				frameInfo.viewerObject.transform.rotation = glm::vec3{ -0.0913868f, 5.08866f, 0 };
				frameInfo.viewerObject.transform.translation = glm::vec3{ 98.9157f, -24.5118f, -135.239f };
				int i = 0;
				for (auto& kv : frameInfo.gameObjects) { // key value
					auto& obj = kv.second;
					obj.transform.translation = obj.baseTransform.translation;
				}
				planetSize = 1;
				timeSpeed = 0;
			};
			if (ImGui::Button(u8"Zobrazit Zemi a Měsíc se Sluncem v pozadí")) {
				frameInfo.viewerObject.transform.translation = { -152.f, 0.f, -2.f }; 
				frameInfo.viewerObject.transform.rotation = { 0.f, 45.2f, 0.f };
				int i = 0;
				for (auto& kv : frameInfo.gameObjects) { // key value
					auto& obj = kv.second;
					obj.transform.translation = obj.baseTransform.translation;
				}
				planetSize = 1;
				timeSpeed = 0;
			};
			if (ImGui::Button(u8"Porovnání velikostí planet - v řadě")) {
				frameInfo.viewerObject.transform.rotation = glm::vec3{ -1.12992f, 6.27481f, 0 };
				frameInfo.viewerObject.transform.translation = glm::vec3{ -2904.f, -3566.f, -1615.26f };
				int i = 0;
				for (auto& kv : frameInfo.gameObjects) { // key value
					auto& obj = kv.second;
					obj.transform.translation = obj.baseTransform.translation;
				}
				planetSize = 100;
				timeSpeed = 0;
			};
			if (ImGui::Button(u8"Porovnání velikostí planet - za sebou")) {
				frameInfo.viewerObject.transform.rotation = glm::vec3{ -0.0253871f, 4.12595f, 0 };
				frameInfo.viewerObject.transform.translation = glm::vec3{ 113.f, -24.5118f, 282.026f };
				int i = 0;
				for (auto& kv : frameInfo.gameObjects) { // key value
					auto& obj = kv.second;
					obj.transform.translation = obj.baseTransform.translation;
				}
				planetSize = 55;
				timeSpeed = 0;
			};
			if (ImGui::Button(u8"Zobrazit planety s výraznými prstenci")) {
				frameInfo.viewerObject.transform.translation = { -2017.34f, 0.f, -1245.f };
				frameInfo.viewerObject.transform.rotation = { 0.017595f, 6.24023f, 0.f };
				auto lst_front = windowsOpened.begin();
				std::advance(lst_front, 6);
				*lst_front = true;
				std::advance(lst_front, 1);
				*lst_front = true;
				int i = 0;
				for (auto& kv : frameInfo.gameObjects) { // key value
					auto& obj = kv.second;
					obj.transform.translation = obj.baseTransform.translation;
				}
				planetSize = 100;
				timeSpeed = 0;
			};
			ImGui::End();
		}
		{
			ImGui::Begin(u8"Nastavení scény", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
			ImGui::SliderInt(u8"Rychlost času", &timeSpeed, -1000, 1000); ImGui::SameLine(); if (ImGui::Button(ICON_FA_PLAY)) { timeSpeed = 1; }; ImGui::SameLine(); if (ImGui::Button(ICON_FA_STOP)) { timeSpeed = 0; };
			ImGui::SliderInt(u8"Měřítko planet", &planetSize, 1, 100);
			ImGui::End();
		}
		{
			ImGui::Begin(u8"Nastavení ovládání", nullptr, ImGuiWindowFlags_NoMove);
			ImGui::SliderFloat(u8"Rychlost pohybu", &moveSpeed, 1, 500);
			ImGui::SliderFloat(u8"Rychlost myši", &lookSpeed, 1, 10);
			ImGui::End();
		}
		{
			float yaw = frameInfo.viewerObject.transform.rotation.y;
			const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
			const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
			const glm::vec3 upDir{ 0.f, -1.f, 0.f };
			glm::vec3 moveDir{ 0.f };
			glm::vec3 rotate{ 0 };

			ImGui::SetNextWindowSize(ImVec2{130, 130});
			ImGui::Begin(u8"Ovládání", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

				// Table (Grid) of controls (3x3)
				ImGui::BeginTable("Kamera", 3);
				ImGui::TableSetupColumn("");
				ImGui::TableSetupColumn("");
				ImGui::TableSetupColumn("");

					ImGui::TableNextColumn();
					ImGui::Button(ICON_FA_UNDO); if (ImGui::IsItemActive()) { rotate.y -= 1.f; } ImGui::TableNextColumn();
					ImGui::Button(ICON_FA_ARROW_UP);  if (ImGui::IsItemActive()) { moveDir += forwardDir; } ImGui::TableNextColumn();
					ImGui::Button(ICON_FA_REPEAT); if (ImGui::IsItemActive()) { rotate.y += 1.f; } ImGui::TableNextColumn();
				
					ImGui::Button(ICON_FA_ARROW_LEFT);  if (ImGui::IsItemActive()) { moveDir -= rightDir; } ImGui::TableNextColumn();
					if (ImGui::Button(ICON_FA_GLOBE)) { frameInfo.viewerObject.transform.translation = { -152.f, 0.f, -2.f }; frameInfo.viewerObject.transform.rotation = { 0.f, 45.2f, 0.f };}; ImGui::TableNextColumn();
					ImGui::Button(ICON_FA_ARROW_RIGHT); if (ImGui::IsItemActive()) { moveDir += rightDir; } ImGui::TableNextColumn();

					ImGui::Button(ICON_FA_LEVEL_DOWN);  if (ImGui::IsItemActive()) { moveDir -= upDir; } ImGui::TableNextColumn();
					ImGui::Button(ICON_FA_ARROW_DOWN); if (ImGui::IsItemActive()) { moveDir -= forwardDir; } ImGui::TableNextColumn();
					ImGui::Button(ICON_FA_LEVEL_UP);  if (ImGui::IsItemActive()) { moveDir += upDir; }

				ImGui::EndTable();

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
				frameInfo.viewerObject.transform.rotation += lookSpeed * frameInfo.FrameTime * glm::normalize(rotate);
			}
			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				frameInfo.viewerObject.transform.translation += moveSpeed * frameInfo.FrameTime * glm::normalize(moveDir);
			}

			ImGui::End();
		}
		{
			ImGui::Begin(u8"Nápověda", nullptr);
			if (ImGui::TreeNode(u8"Ovládání")) {
				if (ImGui::TreeNode(u8"Kamera")) {
					ImGui::TextWrapped(u8"W - posunout kameru dopředu.");
					ImGui::TextWrapped(u8"S - posunout kameru dozadu.");
					ImGui::TextWrapped(u8"A - posunout kameru doleva.");
					ImGui::TextWrapped(u8"D - posunout kameru doprava.");
					ImGui::TextWrapped(u8"Q - posunout kameru vzhůru.");
					ImGui::TextWrapped(u8"E - posunout kameru dolů.");
					ImGui::TextWrapped(u8"Levé tlačítko myši - otočení kamery.");
					ImGui::TreePop();
				}
				if (ImGui::TreeNode(u8"Otevřít menu")) {
					ImGui::TextWrapped(u8"ESC - otevřít / zavřít.");
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"Okna")) {
				ImGui::TextWrapped(u8"Okna s šipkou vlevo se dají skrýt a znovu otevřít.");
				ImGui::TextWrapped(u8"Křížkem se okno zavře.");
				if (ImGui::TreeNode(u8"Seznam těles")) {
					ImGui::TextWrapped(u8"Tlačítko s názvem planety zobrazí informace o planetách.");
					ImGui::TextWrapped(u8"Lupa přiblíži kameru k dané planetě.");
					ImGui::TreePop();
				};
				if (ImGui::TreeNode(u8"Scény")) {
					ImGui::TextWrapped(u8"Po kliknutí na název scény se zobrazí přednastavená scéna.");
					ImGui::TextWrapped(u8"Upraví se velikosti planet a kamera se posune.");
					ImGui::TreePop();
				};
				if (ImGui::TreeNode(u8"Nastavení scény")) {
					if (ImGui::TreeNode(u8"Rychlost času")) {
						ImGui::TextWrapped(u8"Slider - nastavení rychlosti času.");
						ImGui::TextWrapped(u8"Tlačítko play - nastavení rychlosti času na 1.");
						ImGui::TextWrapped(u8"Tlačítko stop - zastavení času.");
						ImGui::TreePop();
					};
					if (ImGui::TreeNode(u8"Měřítko planet")) {
						ImGui::TextWrapped(u8"Zvětší planety podle měřítká. Jejich velikost se vynásobí zadaným číslem.");
					};
					ImGui::TreePop();
				};
				if (ImGui::TreeNode(u8"Nastavení ovládání")) {
					ImGui::TextWrapped(u8"Rychlost pohybu - Změnením hodnoty upravíte rychlost pohybu v prostoru.");
					ImGui::TextWrapped(u8"Rychlost myši - Změnením hodnoty upravíte citlivost otáčení kamery.");
					ImGui::TreePop();
				};
				if (ImGui::TreeNode(u8"Ovládání")) {
					ImGui::TextWrapped(u8"Šipky - pohyb vpřed/vzad/vlevo/vpravo.");
					ImGui::TextWrapped(u8"Šipky s úrovní - pohyb kamery dolů/vzhůru");
					ImGui::TextWrapped(u8"Šipky v kroužku - otočení kamery doleva/doprava.");
					ImGui::TextWrapped(u8"Zeměkoule - vrácení kamery do počáteční pozice.");
					ImGui::TreePop();
				};
				ImGui::TreePop();
			}
			if (ImGui::TreeNode(u8"Ukončit")) {
				ImGui::TextWrapped(u8"ESC - otevřít / zavřít hlavní menu.");
				ImGui::TreePop();
			}
			ImGui::End();
		}
		if (mainMenu) {
			ImGui::Begin(u8"Hlavní menu", &mainMenu, ImGuiWindowFlags_NoMove);
			if (ImGui::Button(u8"Ukončit", ImVec2(ImGui::GetWindowSize().x, 0.0f))) {
				std::exit(EXIT_SUCCESS);
			};
			ImGui::End();
		}
		//if (debug) {
		//	ImGui::Begin(
		//		"Debug",
		//		&debug);  // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//	ImGui::Text(
		//		"Application average %.3f ms/frame (%.1f FPS)",
		//		1000.0f / ImGui::GetIO().Framerate,
		//		ImGui::GetIO().Framerate);
		//
		//	if (ImGui::Button(u8"Zavřít")) debug = false;
		//	ImGui::End();
		//}
	}

	void ExoGui::renderGui(VkCommandBuffer imGuiCommandBuffer) {
		// render the frame we created and occupied
		ImGui::Render();
		ImDrawData* drawdata = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawdata, imGuiCommandBuffer);
	}

	void ExoGui::getDbData() {
		this->planetData = ExoDB::planetData;
	}

	void ExoGui::initWindowState() {
		for (auto& row : this->planetData) {
			windowsOpened.push_back(false);
			// No new elements should be added after this code
			// Find way to lock windowsOpened after this "initiation"
		}
	}

}