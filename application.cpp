#include "application.h"
#include "camera.h"
#include "buffer.h"

// systems
#include "simple_render_system.h"
#include "point_light_system.h"
#include "primitive_model_system.h"
#include "keyboard_mouse_movement_controller.h"
#include "exo_gui.h"
#include "exo_db.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// c++
#include <stdexcept>
#include <array>
#include <string>
#include <functional>
#include <chrono>
#include <iostream>

namespace exo {

	Application::Application() {
		globalPool = ExoDescriptorPool::Builder(device)
			.setMaxSets(ExoSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ExoSwapChain::MAX_FRAMES_IN_FLIGHT)
			//.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ExoSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadObjects();
	}

	Application::~Application() { }

	void Application::run() { // Iterates until the windows is closed
		std::vector<std::unique_ptr<ExoBuffer>> uboBuffers(ExoSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<ExoBuffer>(
				device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uboBuffers[i]->map();
		}
		ExoBuffer globalUboBuffer{
			device,
			sizeof(GlobalUbo),
			ExoSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			device.properties.limits.minUniformBufferOffsetAlignment
		};
		globalUboBuffer.map(); // allocate UboBuffer

		// what should descriptors expect
		auto globalSetLayout = ExoDescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS) // bind to vertex shader
			//.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(ExoSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			//auto imageInfo = ; // NEED TO GET THIS
			ExoDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				//.writeImage(1, &imageInfo)
				.build(globalDescriptorSets[i]);
		}

		// render systems and camera
		SimpleRenderSystem simpleRenderSystem{ device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

		ExoCamera camera{};

		// player
		auto viewerObject = ExoObject::createGameObject();
		viewerObject.transform.translation = { -152.f, 0.f, -2.f }; // start at earth
		viewerObject.transform.rotation = { 0.f, 45.2f, 0.f }; // show beautiful scenery
		KeyboardMouseMovementController cameraController{ window.getGLFWwindow() };
		double previous_window_x;
		double previous_window_y;
		glfwGetCursorPos(window.getGLFWwindow(), &previous_window_x, &previous_window_y);

		// imgui
		ExoGui gui{ window, device, renderer.getSwapChainRenderPass(), renderer.getImageCount(), db };

		// return very accurate now time
		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!window.shouldWindowClose()) { // Keep the window opened until closed
			glfwPollEvents(); // Checks events (like pressing buttons, etc.)

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			//frameTime = glm::min(frameTime, MAX_FRAME_TIME);

			cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject, previous_window_x, previous_window_y);
			glfwGetCursorPos(window.getGLFWwindow(), &previous_window_x, &previous_window_y);

			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = renderer.getAspectRatio();
			// always be kept with aspect ration of window
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10000.f); // last parameter - far plane - render distance

			// GUI Variables
			planetSize = gui.planetSize;
			timeSpeed = gui.timeSpeed;

			// todo
			// begin offscreen shadow pass
			// render shadow casting objects
			// end offscreen shadow pass

			if (auto commandBuffer = renderer.beginFrame()) {
				int frameIndex = renderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					viewerObject,
					camera,
					globalDescriptorSets[frameIndex],
					objects
				};

				gui.newFrame();

				// update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				pointLightSystem.update(frameInfo, ubo);
				update(frameInfo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render frame + gui
				renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				gui.runGui(frameInfo);
				gui.renderGui(commandBuffer);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();

			}

		}

		// cpu will block until all gpu operations are completed
		vkDeviceWaitIdle(device.device()); // fixes the errors at the end
	}

	void Application::update(FrameInfo& frameInfo) {
		int i = 0;
		for (auto& kv : frameInfo.gameObjects) {
			if (i < db.planetData.size()) {
				auto& obj = kv.second;
				if (obj.pointLight != nullptr) continue; // dont rotate Sun

				auto rotatePlanet = glm::rotate(glm::mat4(1.f), frameInfo.FrameTime / stof(db.planetData.at(i).at(8).second) * timeSpeed, { 0.f, -.05f, 0.f });

				obj.transform.translation = glm::vec3(rotatePlanet * glm::vec4(obj.transform.translation, .05f));
				float scale = (((stof(db.planetData.at(i).at(4).second) / earth_base) * earth_model_base) / 100) * planetSize;
				obj.transform.scale = glm::vec3(scale);
				i++;
			}
		}
	}

	void Application::loadObjects() {
		// MODELS
		std::shared_ptr<ExoModel> sphere_model = ExoModel::createModelFromFile(device, "models/earth.obj");
		std::shared_ptr<ExoModel> ring_model = ExoModel::createModelFromFile(device, "models/rings.obj");

		// SUN
		auto sun = ExoObject::makePointLight(1.f);
		sun.transform.translation = { 0.f, 0.f,0.f }; // x - z - y
		float scale = ((109.f / earth_base) * earth_model_base) / 100;
		sun.transform.scale = glm::vec3(50.f);
		objects.emplace(sun.getId(), std::move(sun));


		// PLANETS (db)
		for (auto& row : db.planetData) {
			auto obj = ExoObject::createGameObject();
			obj.model = sphere_model;
			obj.transform.translation = { -stof(row.at(3).second), -stof(row.at(9).second), 0.f };
			float scale = ((stof(row.at(4).second) / earth_base) * earth_model_base) / 100;
			obj.transform.scale = glm::vec3(scale);
			objects.emplace(obj.getId(), std::move(obj));
		}

		// PLANET RINGS
		auto saturn_ring = ExoObject::createGameObject();
		saturn_ring.model = ring_model;
		saturn_ring.transform.translation = { -stof(db.planetData.at(6).at(3).second), -stof(db.planetData.at(6).at(9).second), 0.f };
		saturn_ring.transform.scale = glm::vec3(((stof(db.planetData.at(6).at(4).second) / earth_base) * earth_model_base) / 100);
		saturn_ring.transform.rotation = { 10.f, 0.f, 0.f };
		objects.emplace(saturn_ring.getId(), std::move(saturn_ring));

		auto uran_ring = ExoObject::createGameObject();
		uran_ring.model = ring_model;
		uran_ring.transform.translation = { -stof(db.planetData.at(7).at(3).second), -stof(db.planetData.at(7).at(9).second), 0.f };
		uran_ring.transform.scale = glm::vec3(((stof(db.planetData.at(7).at(4).second) / earth_base) * earth_model_base) / 100);
		uran_ring.transform.rotation = { 10.f, 0.f, 0.f };
		objects.emplace(uran_ring.getId(), std::move(uran_ring));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = ExoObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			objects.emplace(pointLight.getId(), std::move(pointLight));
		}

		// PLANETS (static)
		//auto sun = ExoObject::createGameObject();
		//sun.model = sphere_model;
		//sun.transform.translation = { 0.f, 25.f,0.f }; // x - z - y
		//sun.transform.scale = glm::vec3(109.f);
		//
		//objects.emplace(sun.getId(), std::move(sun));
		//
		//auto mercury = ExoObject::createGameObject();
		//mercury.model = sphere_model;
		//mercury.transform.translation = { -100.f, 25.f, 0.f }; // x - z - y
		//mercury.transform.scale = glm::vec3(0.38f);
		//
		//objects.emplace(mercury.getId(), std::move(mercury));
		//
		//auto venus = ExoObject::createGameObject();
		//venus.model = sphere_model;
		//venus.transform.translation = { -200.f, 25.f, 0.f }; // x - z - y
		//venus.transform.scale = glm::vec3(0.94f);
		//
		//objects.emplace(venus.getId(), std::move(venus));
		//
		//auto earth = ExoObject::createGameObject();
		//earth.model = sphere_model;
		//earth.transform.translation = {-300.f, 25.f,0.f }; // x - z - y
		//earth.transform.scale = glm::vec3(1.f);
		//
		//objects.emplace(earth.getId(), std::move(earth));
		//
		//auto moon = ExoObject::createGameObject();
		//moon.model = sphere_model;
		//moon.transform.translation = { -310.f, 25.f, 0.f }; // x - z - y
		//moon.transform.scale = glm::vec3(0.27f);
		//
		//objects.emplace(moon.getId(), std::move(moon));
		//
		//auto mars = ExoObject::createGameObject();
		//mars.model = sphere_model;
		//mars.transform.translation = { -400.f, 25.f, 0.f }; // x - z - y
		//mars.transform.scale = glm::vec3(0.53f);
		//
		//objects.emplace(mars.getId(), std::move(mars));
		//
		//auto jupiter = ExoObject::createGameObject();
		//jupiter.model = sphere_model;
		//jupiter.transform.translation = { -500.f, 25.f, 0.f }; // x - z - y
		//jupiter.transform.scale = glm::vec3(11.f);
		//
		//objects.emplace(jupiter.getId(), std::move(jupiter));
		//
		//auto saturn = ExoObject::createGameObject();
		//saturn.model = sphere_model;
		//saturn.transform.translation = { -600.f, 25.f, 0.f }; // x - z - y
		//saturn.transform.scale = glm::vec3(9.14f);
		//
		//objects.emplace(saturn.getId(), std::move(saturn));
		//
		//auto uranus = ExoObject::createGameObject();
		//uranus.model = sphere_model;
		//uranus.transform.translation = { -700.f, 25.f, 0.f }; // x - z - y
		//uranus.transform.scale = glm::vec3(4.f);
		//
		//objects.emplace(uranus.getId(), std::move(uranus));
		//
		//auto neptune = ExoObject::createGameObject();
		//neptune.model = sphere_model;
		//neptune.transform.translation = { -800.f, 25.f, 0.f }; // x - z - y
		//neptune.transform.scale = glm::vec3(3.86f);
		//
		//objects.emplace(neptune.getId(), std::move(neptune));
		//
		//auto pluto = ExoObject::createGameObject();
		//pluto.model = sphere_model;
		//pluto.transform.translation = { -900.f, 25.f, 0.f }; // x - z - y
		//pluto.transform.scale = glm::vec3(.3f);
		//
		//objects.emplace(pluto.getId(), std::move(pluto));

	}
}