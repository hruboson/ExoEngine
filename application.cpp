#include "application.h"
#include "camera.h"
#include "buffer.h"

// systems
#include "simple_render_system.h"
#include "primitive_model_system.h"
#include "keyboard_mouse_movement_controller.h"

// libs
#include <stdexcept>
#include <array>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>
#include <iostream>

namespace exo {

	struct GlobalUbo {
		// same as simple push constant data struct
		alignas(16) glm::mat4 projectionView{ 1.f };
		alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 3.f, 0.f, 3.f });
		alignas(16) glm::vec3 pointLight;
	} ubo;

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

		auto globalSetLayout = ExoDescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT) // bind to vertex shader
			.build();
		/*auto samplerSetLayout = ExoDescriptorSetLayout::Builder(device)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();*/

		std::vector<VkDescriptorSet> globalDescriptorSets(ExoSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			ExoDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		// render system and camera
		SimpleRenderSystem simpleRenderSystem{ device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		ExoCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(.0f, .0f, 2.5f));

		// player
		auto viewerObject = ExoObject::createGameObject();
		KeyboardMouseMovementController cameraController{ window.getGLFWwindow() };
		double previous_window_x;
		double previous_window_y;
		glfwGetCursorPos(window.getGLFWwindow(), &previous_window_x, &previous_window_y);

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
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 5000.f); // last parameter - far plane - render distance

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
					camera,
					globalDescriptorSets[frameIndex]
				};
				// update
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
				globalUboBuffer.writeToIndex(&ubo, frameIndex);
				globalUboBuffer.flushIndex(frameIndex);

				// render frame
				renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderObjects(frameInfo, objects);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}

		}

		// cpu will block until all gpu operations are completed
		vkDeviceWaitIdle(device.device()); // fixes the errors at the end
	}

	void Application::loadObjects() {

		// 3D Objects

		std::shared_ptr<ExoModel> sphere_model = ExoModel::createModelFromFile(device, "models/earth.obj");

		auto sun = ExoObject::createGameObject();
		sun.model = sphere_model;
		sun.transform.translation = { 200.f, 25.f, 200.f }; // x - z - y
		sun.transform.scale = glm::vec3(109.f);

		objects.push_back(std::move(sun));

		auto mercury = ExoObject::createGameObject();
		mercury.model = sphere_model;
		mercury.transform.translation = { 100.f, 25.f, 100.f }; // x - z - y
		mercury.transform.scale = glm::vec3(0.38f);

		objects.push_back(std::move(mercury));

		auto venus = ExoObject::createGameObject();
		venus.model = sphere_model;
		venus.transform.translation = { 50.f, 25.f, 50.f }; // x - z - y
		venus.transform.scale = glm::vec3(0.94f);

		objects.push_back(std::move(venus));

		auto earth = ExoObject::createGameObject();
		earth.model = sphere_model;
		earth.transform.translation = {0.f, 25.f,0.f }; // x - z - y
		earth.transform.scale = glm::vec3(1.f);

		objects.push_back(std::move(earth));

		auto moon = ExoObject::createGameObject();
		moon.model = sphere_model;
		moon.transform.translation = { 0.f, 25.f, 5.f }; // x - z - y
		moon.transform.scale = glm::vec3(0.27f);

		objects.push_back(std::move(moon));

		auto mars = ExoObject::createGameObject();
		mars.model = sphere_model;
		mars.transform.translation = { -50.f, 25.f, -50.f }; // x - z - y
		mars.transform.scale = glm::vec3(0.53f);

		objects.push_back(std::move(mars));

		auto jupiter = ExoObject::createGameObject();
		jupiter.model = sphere_model;
		jupiter.transform.translation = { -100.f, 25.f, -100.f }; // x - z - y
		jupiter.transform.scale = glm::vec3(11.f);

		objects.push_back(std::move(jupiter));

		auto saturn = ExoObject::createGameObject();
		saturn.model = sphere_model;
		saturn.transform.translation = { -150.f, 25.f, -150.f }; // x - z - y
		saturn.transform.scale = glm::vec3(9.14f);

		objects.push_back(std::move(saturn));

		auto uranus = ExoObject::createGameObject();
		uranus.model = sphere_model;
		uranus.transform.translation = { -200.f, 25.f, -200.f }; // x - z - y
		uranus.transform.scale = glm::vec3(4.f);

		objects.push_back(std::move(uranus));

		auto neptune = ExoObject::createGameObject();
		neptune.model = sphere_model;
		neptune.transform.translation = { -250.f, 25.f, -250.f }; // x - z - y
		neptune.transform.scale = glm::vec3(3.86f);

		objects.push_back(std::move(neptune));


		/*std::shared_ptr<ExoModel> utah_teapot_model = ExoModel::createModelFromFile(device, "models/teapot.obj");

		auto utah_teapot = ExoObject::createGameObject();
		utah_teapot.model = utah_teapot_model;
		utah_teapot.transform.translation = { .5f, .5f, 2.5f };
		utah_teapot.transform.scale = glm::vec3(3.f);
		utah_teapot.transform.rotation = {90, 0, 90};

		objects.push_back(std::move(utah_teapot));*/
	}

}