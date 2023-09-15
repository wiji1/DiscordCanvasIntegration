#include "include/CanvasAPI.h"
#include <dpp/cluster.h>
#include <future>

extern std::unique_ptr<dpp::cluster> bot;

std::shared_ptr<std::promise<std::string>> CanvasAPI::get_user_profile(const std::string &user_token) {
    auto promise = std::make_shared<std::promise<std::string>>();

    bot->request(
            "https://canvas.instructure.com/api/v1/users/self/profile", dpp::m_get, [&promise](const dpp::http_request_completion_t & cc) {
                promise->set_value(cc.body);
            }, "",
            "application/json",
            {
                    {"Authorization", "Bearer " + user_token }
            }
    );
    return promise;
}

std::shared_ptr<std::promise<std::string>> CanvasAPI::get_user_courses(const std::string &user_token) {
    auto promise = std::make_shared<std::promise<std::string>>();

    bot->request(
            "https://canvas.instructure.com/api/v1/courses?per_page=300",
            dpp::m_get, [&promise](const dpp::http_request_completion_t & cc) {
                promise->set_value(cc.body);
            }, "",
            "application/json",
            {
                    {"Authorization", "Bearer " + user_token }
            }
    );
    return promise;
}

std::shared_ptr<std::promise<std::string>> CanvasAPI::get_course(long course_id, const std::string &user_token) {
    auto promise = std::make_shared<std::promise<std::string>>();

    bot->request(
            "https://canvas.instructure.com/api/v1/courses/" + std::to_string(course_id), dpp::m_get,
            [&promise](const dpp::http_request_completion_t & cc) {
                //TODO: Throw exception here and find a new accessor
                promise->set_value(cc.body);
            }, "",
            "application/json",
            {
                    {"Authorization", "Bearer " + user_token }
            }
    );
    return promise;
}

std::shared_ptr<std::promise<std::string>> CanvasAPI::get_assignments(long course_id, const std::string &user_token) {
    auto promise = std::make_shared<std::promise<std::string>>();

    bot->request(
            "https://canvas.instructure.com/api/v1/courses/" + std::to_string(course_id) + "/assignments?bucket=future", dpp::m_get,
            [&promise](const dpp::http_request_completion_t & cc) {
                //TODO: Throw exception here and find a new accessor
                promise->set_value(cc.body);
            }, "",
            "application/json",
            {
                    {"Authorization", "Bearer " + user_token }
            }
    );
    return promise;
}
