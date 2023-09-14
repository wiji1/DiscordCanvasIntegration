#ifndef DISCORDCANVASINTEGRATION_TRACKEDCOURSE_H
#define DISCORDCANVASINTEGRATION_TRACKEDCOURSE_H

#include "channel.h"
#include "role.h"

class TrackedCourse {
public:
    long course_id;
    long category_id;
    long announcements_channel;
    long forums_channel;
    long role_id;

    explicit TrackedCourse();
    explicit TrackedCourse(long course_id, long category_id, long announcements_channel, long forums_channel, long role_id);

    bool verify_existence(const dpp::role_map &role_map, const dpp::channel_map &channel_map) const;
};


#endif
