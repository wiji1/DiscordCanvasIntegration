#ifndef DISCORDCANVASINTEGRATION_COURSE_H
#define DISCORDCANVASINTEGRATION_COURSE_H

#include <string>
#include <vector>

class Course {
public:
    long course_id;
    std::string name;
    std::vector<long> trackingGuilds;

    explicit Course(long course_id);
    void update();
    void save() const;
    std::string find_accessor();

};


#endif
