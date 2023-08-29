#ifndef DISCORDCANVASINTEGRATION_CONFIGMANAGER_H
#define DISCORDCANVASINTEGRATION_CONFIGMANAGER_H

#include <string>
#include "json.h"

class ConfigManager {
public:
    static nlohmann::json data;
    static void init();
};


#endif
