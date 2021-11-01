#pragma once

#include "window.h"
#include "pipeline.h"
#include "device.h"
#include "renderer.h"
#include "model.h"
#include "object.h"
#include "camera.h"
#include "frame_info.h"

// standard library
#include <memory>

namespace exo {

	class SimpleRenderSystem {

	public:

		SimpleRenderSystem(ExoDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator = (const SimpleRenderSystem&) = delete;

		void renderObjects(FrameInfo &frameInfo, std::vector<ExoObject> &objects);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		ExoDevice &device;

		std::unique_ptr<ExoPipeline> pipeline; // smart pointer -> we dont need to call new and delete
		VkPipelineLayout pipelineLayout;

	};

}