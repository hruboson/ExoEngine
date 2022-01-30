#pragma once

#include "device.h"

namespace exo {

	class ExoTexture {
	public:
		ExoTexture(ExoDevice& device, std::string texturePath);
		~ExoTexture();
	private:
		ExoDevice& device_;

		// textures
		std::string texturePath;
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
	};

}