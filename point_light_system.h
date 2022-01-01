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

	class PointLightSystem {

	public:

		PointLightSystem(ExoDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator = (const PointLightSystem&) = delete;

		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		ExoDevice& device;

		std::unique_ptr<ExoPipeline> pipeline; // smart pointer -> we dont need to call new and delete
		VkPipelineLayout pipelineLayout;

	};

}