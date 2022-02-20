#pragma once

#include "model.h"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>


namespace exo {
	
	struct TransformComponent {
		glm::vec3 translation{}; // position offset (3d)
		glm::vec3 scale{ 1.f, 1.f, 1.f }; // size
		glm::vec3 rotation{}; // rotation

		// this matrix is same as (Ry * Rx * Rz * scale)
		// rotation uses tait-bryan angles with axis order Y(1), X(2), Z(3)
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	class ExoObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, ExoObject>;

		static ExoObject createGameObject();
		static ExoObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		ExoObject(const ExoObject&) = delete;
		ExoObject& operator = (const ExoObject&) = delete;
		ExoObject(ExoObject&&) = default;
		ExoObject& operator = (ExoObject&&) = default;

		id_t getId();

		void setRing(int parent);
		bool isRing();
		int getParent();

		glm::vec3 color{};
		TransformComponent transform{};

		std::shared_ptr<ExoModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;
	private:
		ExoObject(id_t objId);
		id_t id;
		bool ring = false;
		int parent;
	};

}