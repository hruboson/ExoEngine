#pragma once

#include "window.h"

#include <optional> // Helps to decide whether value exists or not
#include <vector>

namespace exo {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
		VkFormat imagesFormat;
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class ExoDevice {
	public:
		// Enables validation layers
	#ifdef NDEBUG // NDEBUG = "not debug" (C++ standard)
		const bool enableValidationLayers = false;
	#else
		const bool enableValidationLayers = true;
	#endif

		ExoDevice(ExoWindow& window);
		~ExoDevice();

		// Not copyable or movable
		ExoDevice(const ExoDevice&) = delete;
		ExoDevice operator=(const ExoDevice&) = delete;
		ExoDevice(ExoDevice&&) = delete;
		ExoDevice& operator=(ExoDevice&&) = delete;

		// getters
		VkCommandPool getCommandPool() { return commandPool; }
		VkDevice device() { return device_; }
		VkSurfaceKHR surface() { return surface_; }
		VkQueue graphicsQueue() { return graphicsQueue_; }
		uint32_t graphicsQueueFamily() { return graphicsQueueFamily_; }
		VkQueue presentQueue() { return presentQueue_; }
		VkInstance instance() { return instance_; }
		VkPhysicalDevice physicalDevice() { return physicalDevice_;  }

		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice_); }
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice_); };
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer Helper Functions
		void createBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& bufferMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void copyBufferToImage(
			VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

		void createImageWithInfo(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory);

		// textures
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void createTextureImage();
		void createTextureImageView();
		VkImageView createImageView(VkImage image, VkFormat format);

		// sampler
		VkSampler textureSampler;
		void createTextureSampler();

		VkPhysicalDeviceProperties properties;

	private:
		void createInstance();
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createCommandPool();

		VkInstance instance_;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
		ExoWindow& window;
		VkCommandPool commandPool;

		VkDevice device_;
		VkSurfaceKHR surface_;
		VkQueue graphicsQueue_;
		uint32_t graphicsQueueFamily_;
		VkQueue presentQueue_;

		// helper functions
		bool isDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void hasGflwRequiredInstanceExtensions();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> deviceExtensions = { // Device extensions such as swapchain
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};
}