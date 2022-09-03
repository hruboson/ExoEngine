#pragma once

#include "device.h"
#include "buffer.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>
#include <memory>

namespace exo {

	// takes vertex data created by cpu or in a file, copy the data from cpu to gpu and render them efficiently
	class ExoModel {
	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			//glm::vec2 texCoord{}; // for texels

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex &other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {

			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		ExoModel(ExoDevice &device, const ExoModel::Builder &builder);
		~ExoModel();

		ExoModel(const ExoModel&) = delete;
		ExoModel& operator = (const ExoModel&) = delete;

		static std::unique_ptr<ExoModel> createModelFromFile(ExoDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		ExoDevice &device;

		std::unique_ptr<ExoBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<ExoBuffer> indexBuffer;
		uint32_t indexCount;
	};

}