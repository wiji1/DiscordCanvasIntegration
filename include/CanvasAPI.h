#ifndef LEARNINGCPP_CANVASAPI_H
#define LEARNINGCPP_CANVASAPI_H

#include <string>
#include <future>


class CanvasAPI {
public:
    static std::shared_ptr<std::promise<std::string>> get_user_profile(const std::string &user_token);
    static std::shared_ptr<std::promise<std::string>> get_user_courses(const std::string &user_token);
};


#endif
