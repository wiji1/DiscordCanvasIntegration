#ifndef LEARNINGCPP_COURSE_H
#define LEARNINGCPP_COURSE_H

#include <string>

class Course {
public:
    explicit Course(long course_id);

    void update();
    void save() const;

    long course_id;
    std::string name;
    std::string user_token;
    long user_id{};

};


#endif
