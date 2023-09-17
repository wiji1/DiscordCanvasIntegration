#ifndef DISCORDCANVASINTEGRATION_COURSE_H
#define DISCORDCANVASINTEGRATION_COURSE_H

#include <string>
#include <vector>
#include <dpp/dpp.h>
#include "User.h"

class Course {
private:
    static std::unordered_map<long, std::shared_ptr<Course>> course_map;
public:
    long course_id;
    bool is_active {true};

    std::string name;
    std::vector<long> tracking_guilds;
    std::vector<long> recent_assignments;
    std::vector<long> recent_announcements;

    dpp::task<void> update(const std::string &override_token = "");
    void save() const;
    void remove() const;
    User &find_accessor();

    static std::shared_ptr<Course> &get_or_create(long course_id, const std::string& access_token);
    static std::shared_ptr<Course> &get_course(long course_id);

    explicit Course(long course_id, const std::string& access_token);
    explicit Course(long discord_id);
};


#endif
