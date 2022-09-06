#include "window.h"
#include "stb_image.h"
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
		window = glfwCreateWindow(this->width, this->height, this->windowName.c_str(), glfwGetPrimaryMonitor(), nullptr); // Create actual window
		//                               |w|          |h|               |title|       |monitor||unrelevant without OpenGL|
		glfwSetWindowUserPointer(window, this);

		// set window icon
		GLFWimage images[1];
		images[0].pixels = stbi_load("exo.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
		glfwSetWindowIcon(window, 1, images);
		stbi_image_free(images[0].pixels);

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
