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
            "https://canvas.instructure.com/api/v1/courses", dpp::m_get, [&promise](const dpp::http_request_completion_t & cc) {
                promise->set_value(cc.body);
            }, "",
            "application/json",
            {
                    {"Authorization", "Bearer " + user_token }
            }
    );
    return promise;
}
