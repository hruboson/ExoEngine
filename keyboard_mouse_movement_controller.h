#pragma once

#include "object.h"
#include "window.h"

namespace exo {

	class KeyboardMouseMovementController {

	public:

		struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
            int drag = GLFW_MOUSE_BUTTON_LEFT;
		};

        void moveInPlaneXZ(GLFWwindow* window, float dt, ExoObject& object, double previous_window_x, double previous_window_y);

        bool mouseButtonPressed;

        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 5.f }; 
	};

}