#include "include/User.h"
#include "json.h"
#include <sstream>
#include <vector>
#include "include/CanvasAPI.h"

User::User(const std::string &user_token, long discord_id) : discord_id {discord_id} {

    auto user_promise = CanvasAPI::get_user_profile(user_token);
    std::stringstream profile_stream {user_promise->get_future().get()};
    nlohmann::json profile_data;

    profile_stream >> profile_data;

    user_id = {profile_data["id"]};
    name = {profile_data["name"]};

    auto courses_promise = CanvasAPI::get_user_courses(user_token);
    std::stringstream courses_stream {courses_promise->get_future().get()};
    nlohmann::json courses_data;

    courses_stream >> courses_data;

    for(const auto &course : courses_data) {
        courses.push_back(course["id"]);
    }

}
