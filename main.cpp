#define GLFW_INCLUDE_VULKAN // 
#include <GLFW/glfw3.h> // GLFW is showing content on screen and includes the Vulkan header inside of itself

#include "application.h"

#include <iostream> // Included for reporting and propagating errors

int main() {
	exo::Application app{};

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}