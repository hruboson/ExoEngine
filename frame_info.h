#pragma once

#include "camera.h"
#include "object.h"

//lib
#include <vulkan/vulkan.h>

namespace exo {
	struct FrameInfo {

		int frameIndex;
		float FrameTime;
		VkCommandBuffer commandBuffer;
		ExoCamera& camera;

		VkDescriptorSet globalDescriptorSet;
		ExoObject::Map& gameObjects;

	};
}