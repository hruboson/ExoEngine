#pragma once

#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "model.h"

// standard library
#include <memory>

namespace exo {

	class ExoRenderer {

	public:
		ExoRenderer(ExoWindow& window, ExoDevice& device);
		~ExoRenderer();

		ExoRenderer(const ExoRenderer&) = delete;
		ExoRenderer& operator = (const ExoRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return swapchain->getRenderPass(); };
		float getAspectRatio() const { return swapchain->extentAspectRatio(); };
		uint32_t getImageCount() { return swapchain->imageCount(); };

		bool isFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex]; 
		};

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		ExoWindow& window;
		ExoDevice& device;
		std::unique_ptr<ExoSwapChain> swapchain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};

}