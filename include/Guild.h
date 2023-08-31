#ifndef DISCORDCANVASINTEGRATION_GUILD_H
#define DISCORDCANVASINTEGRATION_GUILD_H

#include <vector>
#include <unordered_map>
#include "TrackedCourse.h"
#include "include/Course.h"
#include <vector>

class Guild {
private:
    static std::unordered_map<long, std::unique_ptr<Guild>> guild_map;
public:
    std::vector<TrackedCourse> tracked_courses;
    std::vector<long> verified_users;
    long guild_id;
    long verified_role_id;

    void add_tracked_course(long course_id);
    void remove_tracked_course(long course_id);
    void create_verified_role();

    static void register_guild(long guild_id);
    static Guild get_guild(long guild_id);
    void update();

    static std::vector<Guild> get_tracking_guilds(Course &course);

    explicit Guild(long guild_id);
};


#endif
