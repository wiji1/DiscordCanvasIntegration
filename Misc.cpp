#include <sstream>
#include <iostream>
#include <ctime>
#include <iomanip>
#include "include/Misc.h"

long Misc::string_to_time(const std::string &string) {
    std::string timestamp = "2018-06-01T07:00:00Z";

    std::tm parsedTime = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&parsedTime, "%Y-%m-%dT%H:%M:%SZ");

    if (ss.fail()) {
        std::cerr << "Failed to parse timestamp." << std::endl;
        return 1;
    }

    std::time_t parsedTimestamp = std::mktime(&parsedTime);

    if (parsedTimestamp == -1) {
        std::cerr << "Failed to convert parsed time to timestamp." << std::endl;
        return 1;
    }

    return parsedTimestamp;
}
