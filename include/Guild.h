#ifndef DISCORDCANVASINTEGRATION_GUILD_H
#define DISCORDCANVASINTEGRATION_GUILD_H

#include <vector>
#include <unordered_map>
#include "TrackedCourse.h"
#include "include/Course.h"

class Guild {
private:
    static std::unordered_map<long, std::unique_ptr<Guild>> guild_map;
public:
    std::vector<TrackedCourse> tracked_courses;

    void add_tracked_course(long course_id);
    void remove_tracked_course(long course_id);

    //TODO: Get tracking guilds by reference
};


#endif
