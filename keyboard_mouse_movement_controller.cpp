#include "keyboard_mouse_movement_controller.h"
#include <iostream>

namespace exo {

	void KeyboardMouseMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, ExoObject& object, double previous_window_x, double previous_window_y) {
		glm::vec3 rotate{ 0 };

		if (glfwGetMouseButton(window, keys.drag) == GLFW_PRESS) {
			mouseButtonPressed = true;	
		}
		if (mouseButtonPressed) {
			if (glfwGetMouseButton(window, keys.drag) == GLFW_RELEASE) {
				mouseButtonPressed = false;
			}
			else {
				double window_x;
				double window_y;
				glfwGetCursorPos(window, &window_x, &window_y);
				if (previous_window_y - window_y > 0) {
					rotate.x -= (previous_window_y - window_y);
				}
				else if (previous_window_y - window_y < 0) {
					rotate.x += std::abs(previous_window_y - window_y);
				}
				if (previous_window_x - window_x > 0) {
					rotate.y += (previous_window_x - window_x);
				}
				else if (previous_window_x - window_x < 0){
					rotate.y -= std::abs(previous_window_x - window_x);
				}
			}
		}
		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			object.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		// limit pitch values between about +/- 85 degrees
		object.transform.rotation.x = glm::clamp(object.transform.rotation.x, -1.5f, 1.5f);
		object.transform.rotation.y = glm::mod(object.transform.rotation.y, glm::two_pi<float>());

		float yaw = object.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			object.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
		}
	}

}