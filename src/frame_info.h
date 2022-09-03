#pragma once

#include "camera.h"
#include "object.h"
#include "exo_db.h"

//lib
#include <vulkan/vulkan.h>

#define MAX_LIGHTS 10

using queryRow = std::vector<std::pair<std::string, std::string>>;
using queryData = std::vector<queryRow>;

namespace exo {
	struct PointLight {
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct FrameInfo {

		int frameIndex;
		float FrameTime;
		VkCommandBuffer commandBuffer;
		ExoObject& viewerObject;
		ExoCamera& camera;

		VkDescriptorSet globalDescriptorSet;
		ExoObject::Map& gameObjects;
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, 0.02f }; // w = light intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};
}