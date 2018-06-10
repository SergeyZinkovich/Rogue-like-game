#pragma once
#include "json.hpp"
#include <fstream>

class ConfigReader {
public:
	nlohmann::json configJson;

	ConfigReader() {
		std::ifstream configStream("config.json");
		configStream >> configJson;
	}
};