#ifndef DISCORDCANVASINTEGRATION_GUILD_H
#define DISCORDCANVASINTEGRATION_GUILD_H

#include <vector>
#include "TrackedCourse.h"

class Guild {
public:
    std::vector<TrackedCourse> tracked_courses;

    void add_tracked_course(long course_id);
    void remove_tracked_course(long course_id);
};


#endif
