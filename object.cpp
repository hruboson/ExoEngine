#include "object.h"
using id_t = unsigned int;

namespace exo {

	glm::mat4 TransformComponent::mat4() {
		auto transform = glm::translate(glm::mat4{ 1.f }, translation);

		// rotation of all axis
		// can be optimized
		transform = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
		transform = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
		transform = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });

		transform = glm::scale(transform, scale);
		return transform;
	}

	glm::mat3 TransformComponent::normalMatrix() {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 invScale = 1.0f / scale;

		return glm::mat3{
			{
				invScale.x * (c1 * c3 + s1 * s2 * s3),
				invScale.x * (c2 * s3),
				invScale.x * (c1 * s2 * s3 - c3 * s1),
			},
			{
				invScale.y * (c3 * s1 * s2 - c1 * s3),
				invScale.y * (c2 * c3),
				invScale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				invScale.z * (c2 * s1),
				invScale.z * (-s2),
				invScale.z * (c1 * c2),
			},
		};
	}

	ExoObject::ExoObject(id_t objId) : id{ objId } {}

	ExoObject ExoObject::createGameObject() {
		static id_t currentId = 0;
		return ExoObject{ currentId++ };
	}

	ExoObject ExoObject::makePointLight(float intensity, float radius, glm::vec3 color)
	{
		ExoObject obj = ExoObject::createGameObject();
		obj.color = color;
		obj.transform.scale.x = radius;
		obj.pointLight = std::make_unique<PointLightComponent>();
		obj.pointLight->lightIntensity = intensity;
		return obj;
	}

	id_t ExoObject::getId() {
		return id;
	}

}