#include <iostream>
#include "include/TrackedCourse.h"

TrackedCourse::TrackedCourse(long course_id, long category_id, long announcements_channel, long forums_channel, long role_id, std::unordered_map<long, long> assignment_map) :
course_id {course_id}, category_id {category_id}, announcements_channel {announcements_channel}, forums_channel {forums_channel},
role_id {role_id}, assignment_map {assignment_map} {

}

TrackedCourse::TrackedCourse() : course_id {0}, category_id {0}, announcements_channel {0}, forums_channel {0}, role_id {0} {

}

bool TrackedCourse::verify_role_existence(const dpp::role_map &role_map) const {
    if(role_map.find(role_id) == role_map.end()) {
       return false;
    }

    return true;
}

bool TrackedCourse::verify_channel_existence(const dpp::channel_map &channel_map) const {
    std::vector<long> channel_ids = {category_id, announcements_channel, forums_channel};

    bool channel_missing = false;

    for(const long &channel_id : channel_ids) {
        if(channel_map.find(channel_id) == channel_map.end()) {
            channel_missing = true;
            break;
        }
    }

    return !channel_missing;
}
