#include "include/CanvasAPI.h"
#include <dpp/cluster.h>
#include <future>

extern std::unique_ptr<dpp::cluster> bot;

dpp::task<std::string> CanvasAPI::get_user_profile(const std::string &user_token) {
    dpp::http_request_completion_t response = co_await bot->co_request("https://canvas.instructure.com/api/v1/users/self/profile",
    dpp::m_get, "", "application/json",
    {
        {"Authorization", "Bearer " + user_token }
    });

   co_return response.body;
}

dpp::task<std::string> CanvasAPI::get_user_courses(const std::string &user_token) {
    dpp::http_request_completion_t response = co_await bot->co_request("https://canvas.instructure.com/api/v1/courses?per_page=300",
        dpp::m_get, "", "application/json",
        {
            {"Authorization", "Bearer " + user_token }
        });
    co_return response.body;
}

dpp::task<std::string> CanvasAPI::get_course(long course_id, const std::string &user_token) {
    dpp::http_request_completion_t response = co_await bot->co_request("https://canvas.instructure.com/api/v1/courses/" +
            std::to_string(course_id), dpp::m_get, "",
            "application/json",
            {
                {"Authorization", "Bearer " + user_token }
            });
    co_return response.body;
}

dpp::task<std::string> CanvasAPI::get_assignments(long course_id, const std::string &user_token) {
    dpp::http_request_completion_t response = co_await bot->co_request("https://canvas.instructure.com/api/v1/courses/" +
            std::to_string(course_id) + "/assignments?bucket=future", dpp::m_get,
    "", "application/json",
    {
        {"Authorization", "Bearer " + user_token }
    });
    co_return response.body;
}

dpp::task<std::string> CanvasAPI::get_announcements(long course_id, const std::string &user_token) {
    dpp::http_request_completion_t response = co_await bot->co_request("https://canvas.instructure.com/api/v1/courses/" +
            std::to_string(course_id) + "/discussion_topics?only_announcements=true&per_page=5", dpp::m_get,
    "", "application/json",
    {
        {"Authorization", "Bearer " + user_token}
    });
    co_return response.body;
}