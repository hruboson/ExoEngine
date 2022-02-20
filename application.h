#pragma once

#include "window.h"
#include "device.h"
#include "renderer.h"
#include "descriptors.h"
#include "model.h"
#include "object.h"
#include "primitive_model_system.h"
#include "frame_info.h"
#include "exo_db.h"

// std
#include <memory>

namespace exo {

	class Application {

	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		void run();
		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator = (const Application&) = delete;

	private:
		void loadObjects();
		void update(FrameInfo& frameInfo);
		// order of declaration matters !!!
		// initializing from top to bottom and destroying in reverse (bottom to top - first objects last window)
		ExoWindow window{ WIDTH, HEIGHT, "ExoPlanets" }; // initialization of an unnamed temporary with a braced-init-list (https://en.cppreference.com/w/cpp/language/list_initialization)
		ExoDevice device{ window };
		ExoRenderer renderer{ window, device };
		PrimitiveModelSystem primitiveModelSystem{ device };

		std::unique_ptr<ExoDescriptorPool> globalPool{};
		std::unique_ptr<ExoDescriptorPool> samplerPool{};

		ExoObject::Map objects;

		// SQLITE DB
		ExoDB db{};
		std::vector<std::vector<std::pair<std::string, std::string>>> planetData;

		// COMPONENTS
		int timeSpeed;
		int planetSize;
		float moveSpeed;
		float lookSpeed;

		// CONSTANTS
		const int earth_base = 12756;
		const int earth_model_base = 10; // base size of earth in engine units
	};

}