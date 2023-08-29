#ifndef DISCORDCANVASINTEGRATION_COURSE_H
#define DISCORDCANVASINTEGRATION_COURSE_H

#include <string>
#include <vector>
#include "include/User.h"

class Course {
private:
    static std::unordered_map<long, std::unique_ptr<Course>> course_map;
public:
    long course_id;
    std::string name;
    std::vector<long> trackingGuilds;

    void update();
    void save() const;
    User &find_accessor();

    static Course &get_or_create(long course_id, const std::string& access_token);
    static Course &get_course(long course_id);

    explicit Course(long course_id, const std::string& access_token);
    explicit Course(long discord_id);


};


#endif
