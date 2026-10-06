#include "Log.h"

namespace Yue {
	void Log::Init() {

	}

	void Log::CoreInfo(const std::string& message) {
		std::cout << "[Yue Engine] " << message << std::endl;
	}

	void Log::Info(const std::string& message) {
		std::cout << "[APP] " << message << std::endl;
	}

	void Log::CoreError(const std::string& message) {
		std::cerr << "[Yue Engine Error] " << message << std::endl;
	}

	void Log::Error(const std::string& message) {
		std::cerr << "[APP Error] " << message << std::endl;
	}
}


