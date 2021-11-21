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

	class ExoObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, ExoObject>;

		static ExoObject createGameObject();

		ExoObject(const ExoObject&) = delete;
		ExoObject& operator = (const ExoObject&) = delete;
		ExoObject(ExoObject&&) = default;
		ExoObject& operator = (ExoObject&&) = default;

		id_t getId();

		std::shared_ptr<ExoModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		ExoObject(id_t objId);
		id_t id;
	};

}