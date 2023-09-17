#ifndef DISCORDCANVASINTEGRATION_CANVASAPI_H
#define DISCORDCANVASINTEGRATION_CANVASAPI_H

#include <string>
#include <future>
#include <dpp/dpp.h>


class CanvasAPI {
public:
    static dpp::task<std::string> get_user_profile(const std::string &user_token);
    static dpp::task<std::string> get_user_courses(const std::string &user_token);
    static dpp::task<std::string> get_course(long course_id, const std::string &user_token);
    static dpp::task<std::string> get_assignments(long course_id, const std::string &user_token);
    static dpp::task<std::string> get_announcements(long course_id, const std::string &user_token);
};


#endif
