#include "include/Guild.h"
#include "main.cpp"
#include <vector>

Guild::Guild(long guild_id) : guild_id {guild_id} {
    verified_role_id = {0};
    create_verified_role();
}

void Guild::add_tracked_course(long course_id) {

}

void Guild::remove_tracked_course(long course_id) {

}

std::vector<Guild> Guild::get_tracking_guilds(Course &course) {
    std::vector<Guild> guildsTrackingCourse;

    for(const auto &guild_pair : guild_map) {
        Guild guild = *guild_pair.second;

        if(guild.is_tracking(course)) guildsTrackingCourse.push_back(guild);
    }
    return guildsTrackingCourse;
}

void Guild::update() {
    auto guild {dpp::find_guild(guild_id)};


}

void Guild::create_verified_role() {
    verified_role_id = 0;
}

void Guild::register_guild(long guild_id) {
    guild_map[guild_id] = std::make_unique<Guild>(guild_id);
}

Guild Guild::get_guild(long guild_id) {
    return Guild(0);
}

bool Guild::is_tracking(const Course &course) {
    for(const auto &tracked_course : tracked_courses) {
        if (tracked_course.first == course.course_id) {
            return true;
        }
    }

    return false;
}
