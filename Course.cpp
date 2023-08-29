#include <dpp/dpp.h>
#include "include/Course.h"

Course::Course(long course_id) : course_id {course_id} {

}

void Course::save() const {

}

void Course::update() {

}

std::string Course::find_accessor() {
    for(const auto &guild_id: trackingGuilds) {
        for(const auto &item: dpp::find_guild(guild_id)->members) {

        }
    }
    return "";
}
