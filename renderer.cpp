#include "renderer.h"
#include <stdexcept>
#include <array>
#include <string>

namespace exo {

	ExoRenderer::ExoRenderer(ExoWindow& window, ExoDevice& device) : window{ window }, device{device} {
		recreateSwapChain();
		createCommandBuffers();
	}

	ExoRenderer::~ExoRenderer() {
		freeCommandBuffers();
	}


	void ExoRenderer::createCommandBuffers() {
		commandBuffers.resize(ExoSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		// primary command buffers can be executed, secondary can be called by other buffers
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void ExoRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void ExoRenderer::recreateSwapChain() {
		auto extent = window.getExtent();
		while (extent.width == 0 || extent.height == 0) { // if window is minimized
			extent = window.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device.device());

		if (swapchain == nullptr) {
			swapchain = std::make_unique<ExoSwapChain>(device, extent);
		}
		else {
			std::shared_ptr<ExoSwapChain> oldSwapChain = std::move(swapchain);
			swapchain = std::make_unique<ExoSwapChain>(device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*swapchain.get())) {
				throw std::runtime_error("Swap chain image (or depth) format has changed");
			}

			if (swapchain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		//
	}

	VkCommandBuffer ExoRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");
		auto result = swapchain->acquireNextImage(&currentImageIndex);

		// to repair - many errors popping while the program is running // REPAIRED
		//std::string string = std::to_string(imageIndex);
		//printf(string.c_str(), '\n');

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// beginInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		return commandBuffer;
	};

	void ExoRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame is not in progreess");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}

		auto result = swapchain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
			window.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % ExoSwapChain::MAX_FRAMES_IN_FLIGHT;
	};

	void ExoRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
 		assert(isFrameStarted && "Can't call beginSwapchainRenderPass while already in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");


		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->getRenderPass();
		renderPassInfo.framebuffer = swapchain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

		// clear color is color where nothing is rendered (void)
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.145f, 0.145f, 0.149f, 1.0f }; // old ide background: { 0.156f, 0.164f, 0.211f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// combine scissors and viewport
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, swapchain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	};

	void ExoRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass while already in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	};
}