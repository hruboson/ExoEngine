#include "primitive_model_system.h"

namespace exo {

	PrimitiveModelSystem::PrimitiveModelSystem(ExoDevice& device) : device{ device } { }
	PrimitiveModelSystem::~PrimitiveModelSystem() { return; }

	// No longer valid with 3D
	// To-do: remake these to 3D
	/*std::unique_ptr<ExoModel> PrimitiveModelSystem::createCircleModel(unsigned int numSides) {
		std::vector<ExoModel::Vertex> uniqueVertices{};
		for (size_t i = 0; i < numSides; i++) {
			float angle = i * glm::two_pi<float>() / numSides;
			uniqueVertices.push_back({ {glm::cos(angle), glm::sin(angle)} });
		}
		uniqueVertices.push_back({});  // adds center vertex at 0, 0

		std::vector<ExoModel::Vertex> vertices{};
		for (size_t i = 0; i < numSides; i++) {
			vertices.push_back(uniqueVertices[i]);
			vertices.push_back(uniqueVertices[(i + 1) % numSides]);
			vertices.push_back(uniqueVertices[numSides]);
		}

		return std::make_unique<ExoModel>(device, vertices);
	}

	std::unique_ptr<ExoModel> PrimitiveModelSystem::createSquareModel(glm::vec2 offset) {
		std::vector<ExoModel::Vertex> vertices = {
			{{-0.5f, -0.5f}},
			{{0.5f, 0.5f}},
			{{-0.5f, 0.5f}},
			{{-0.5f, -0.5f}},
			{{0.5f, -0.5f}},
			{{0.5f, 0.5f}},  //
		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<ExoModel>(device, vertices);
	}

	std::unique_ptr<ExoModel> PrimitiveModelSystem::createTriangleModel(glm::vec2 offset) {
		std::vector<ExoModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<ExoModel>(device, vertices);
	}*/

	// temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
	std::unique_ptr<ExoModel> PrimitiveModelSystem::createCubeModel(glm::vec3 offset) {
		ExoModel::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<ExoModel>(device, modelBuilder);
	}
}