﻿#include "exo_gui.h"
#include "IconsFontAwesome4.h"
#include "frame_info.h"

#include <stdexcept>
#include <utility>

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
		io.Fonts->AddFontFromFileTTF("dearimgui/consola.ttf", 20, NULL, ranges.Data);

		// Icons
		ImFontConfig config;
		config.MergeMode = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromFileTTF("dearimgui/fontawesome-webfont.ttf", 20, &config, icon_ranges);

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

	void ExoGui::newFrame() {
		// we tell imgui we are creating new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ExoGui::runGui(FrameInfo frameInfo) {
		{ // new scope
			ImGui::Begin(u8"Seznam těles");  // Begin must be closed with End
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
			ImGui::Checkbox("Debug", &debug);
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
						ImGui::Begin(row.at(0).second.c_str(), &(*lst_front), ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar); // NO WAAAAAAAY IM GENIUS &(*lst_front)
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
			ImGui::Begin(u8"Nastavení scény", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
			ImGui::SliderInt(u8"Rychlost času", &timeSpeed, 1, 100); ImGui::SameLine(); if (ImGui::Button(ICON_FA_PLAY)) { timeSpeed = 1; }; ImGui::SameLine(); if (ImGui::Button(ICON_FA_STOP)) { timeSpeed = 0; };
			ImGui::SliderInt(u8"Velikost planet", &planetSize, 1, 100);
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
			ImGui::Begin(u8"Ovládání", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |  ImGuiWindowFlags_NoScrollbar);

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
				frameInfo.viewerObject.transform.rotation += 4.f * frameInfo.FrameTime * glm::normalize(rotate);
			}
			if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
				frameInfo.viewerObject.transform.translation += 8.f * frameInfo.FrameTime * glm::normalize(moveDir);
			}

			ImGui::End();
		}
		{
			ImGui::Begin(u8"Scénáře", nullptr);
			ImGui::Button(u8"Porovnání planet");
			ImGui::Button(u8"Skutečná velikost");
			ImGui::Button(u8"Zobrazit celou Sluneční soustavu");
			ImGui::Button(u8"...");
			ImGui::End();
		}
		if (debug) {
			ImGui::Begin(
				"Debug",
				&debug);  // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text(
				"Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);

			if (ImGui::Button(u8"Zavřít")) debug = false;
			ImGui::End();
		}
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