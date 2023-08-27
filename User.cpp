#include "include/User.h"
#include "include/CanvasAPI.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include <json.h>
#include <sstream>
#include <bsoncxx/document/view.hpp>

User::User(const std::string &user_token, long discord_id) : discord_id {discord_id} {
    std::cout << "Test" << std::endl;
    auto user_promise = CanvasAPI::get_user_profile(user_token);
    std::stringstream profile_stream {user_promise->get_future().get()};
    nlohmann::json profile_data;

    profile_stream >> profile_data;

    this->user_token = {user_token};
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

User::User(long discord_id) : discord_id {discord_id} {
    bsoncxx::document::view document{DatabaseManager::fetch_user_document(discord_id)};

    user_token = {document["token"].get_string().value.to_string()};
    user_id = {document["_id"].get_int64()};
    name = document["name"].get_string().value.to_string();

    auto course_array{document["courses"]};
    bsoncxx::array::view course_view = course_array.get_array();

    for (const auto &course_value: course_view) {
        courses.push_back(course_value.get_int64());
    }

}


void User::save() const {
    try {
        DatabaseManager::fetch_user_document(discord_id);
    } catch (DocumentNotFoundException &ex) {
        DatabaseManager::insert_user(*this);
        return;
    }

    DatabaseManager::update_user(*this);
}

