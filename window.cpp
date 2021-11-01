#include "window.h"
#include <stdexcept>

namespace exo {

	ExoWindow::ExoWindow(int width, int height, std::string name) : width{ width }, height{ height }, windowName{ name }{
		initWindow();
	}

	ExoWindow::~ExoWindow() {
		glfwDestroyWindow(window); // Dealocate window
		glfwTerminate();
	}

	void ExoWindow::initWindow() {
		glfwInit(); // Initializes GLFW library

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not create OpenGL context
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		window = glfwCreateWindow(this->width, this->height, this->windowName.c_str(), nullptr, nullptr); // Create actual window
		//                               |w|          |h|               |title|       |monitor||unrelevant without OpenGL|
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void ExoWindow::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}	
	}

	void ExoWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto exoWindow = reinterpret_cast<ExoWindow*>(glfwGetWindowUserPointer(window));

		exoWindow->framebufferResized = true;
		exoWindow->height = height;
		exoWindow->width = width;
	}

}
