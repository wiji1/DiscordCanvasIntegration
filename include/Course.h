#ifndef LEARNINGCPP_COURSE_H
#define LEARNINGCPP_COURSE_H

#include <string>
#include <vector>

class Course {
public:
    explicit Course(long course_id);

    void update();
    void save() const;
    std::string find_accessor();

    long course_id;
    std::string name;
    std::vector<long> trackingGuilds;

};


#endif
