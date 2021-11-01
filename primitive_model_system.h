#pragma once

#include "device.h"
#include "model.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <memory>

namespace exo {


	class PrimitiveModelSystem {

	public:
		PrimitiveModelSystem(ExoDevice& device);
		~PrimitiveModelSystem();

		PrimitiveModelSystem(const PrimitiveModelSystem&) = delete;
		PrimitiveModelSystem& operator = (const PrimitiveModelSystem&) = delete;

		// 2D
		std::unique_ptr<ExoModel> createCircleModel(unsigned int numSides);
		std::unique_ptr<ExoModel> createSquareModel(glm::vec2 offset);
		std::unique_ptr<ExoModel> createTriangleModel(glm::vec2 offset);

		// 3D
		std::unique_ptr<ExoModel> createCubeModel(glm::vec3 offset);

	private:
		ExoDevice& device;

	};


}