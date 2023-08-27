#include <fstream>
#include <iostream>
#include "include/ConfigManager.h"

nlohmann::json ConfigManager::data;

void ConfigManager::init() {
    std::ifstream input {"../resources/config.json"};
    if (!input)
        throw std::runtime_error {"Cannot locate config file"};
    input >> data;
    input.close();
}
