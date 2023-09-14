#ifndef DISCORDCANVASINTEGRATION_GUILD_H
#define DISCORDCANVASINTEGRATION_GUILD_H

#include <vector>
#include <unordered_map>
#include "TrackedCourse.h"
#include "Course.h"
#include "cluster.h"
#include <vector>
#include <bsoncxx/array/value.hpp>

extern std::unique_ptr<dpp::cluster> bot;

class Guild {
public:
    static const int MAXIMUM_ROLE_REQUESTS = 3;

    static std::unordered_map<long, std::shared_ptr<Guild>> guild_map;
    std::vector<std::shared_ptr<TrackedCourse>> tracked_courses;
    std::vector<long> verified_users;

    long guild_id;
    long verified_role_id;
    int used_roles = 0;

    void add_tracked_course(long course_id);
    void remove_tracked_course(const std::shared_ptr<TrackedCourse>& tracked_course);
    void verify_existence();
    void create_verified_role();
    void deregister();
    bool is_tracking(const Course &course);

    static void register_guild(long guild_id);
    static std::shared_ptr<Guild> &get_guild(long guild_id);


    void update();
    void save() const;

    static std::vector<Guild> get_tracking_guilds(Course &course);
    static bool is_registered(long guild_id);

    void document_init(const bsoncxx::document::value &document);

    explicit Guild(const bsoncxx::document::value& document);
    explicit Guild(long guild_id);
};


#endif
