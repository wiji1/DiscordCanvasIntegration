#ifndef DISCORDCANVASINTEGRATION_CANVASAPI_H
#define DISCORDCANVASINTEGRATION_CANVASAPI_H

#include <string>
#include <future>


class CanvasAPI {
public:
    static std::shared_ptr<std::promise<std::string>> get_user_profile(const std::string &user_token);
    static std::shared_ptr<std::promise<std::string>> get_user_courses(const std::string &user_token);
    static std::shared_ptr<std::promise<std::string>> get_course(long course_id, const std::string &user_token);
    static std::shared_ptr<std::promise<std::string>> get_assignments(long course_id, const std::string &user_token);
    static std::shared_ptr<std::promise<std::string>> get_announcements(long course_id, const std::string &user_token);
};


#endif
