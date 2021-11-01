#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace exo {

	class ExoWindow {
	public:
		ExoWindow(int width, int height, std::string name);
		~ExoWindow();

		ExoWindow(const ExoWindow&) = delete;
		ExoWindow& operator = (const ExoWindow&) = delete;

		bool shouldWindowClose() { return glfwWindowShouldClose(window); };

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; };

		void createSurface(VkInstance instance, VkSurfaceKHR* surface);
		bool wasWindowResized() { return framebufferResized; };
		void resetWindowResizedFlag() { framebufferResized = false; };

		GLFWwindow* getGLFWwindow() const { return window; };

	private:
		void initWindow();

		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		int height;
		int width;

		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};

}