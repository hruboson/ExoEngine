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
		ExoObject& viewerObject;
		ExoCamera& camera;

		VkDescriptorSet globalDescriptorSet;
		ExoObject::Map& gameObjects;

	};
}