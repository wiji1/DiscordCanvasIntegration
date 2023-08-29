#include "include/User.h"
#include "include/CanvasAPI.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include "include/Misc.h"
#include <json.h>
#include <sstream>
#include <bsoncxx/document/view.hpp>
#include <utility>

User::User(std::string user_token, long discord_id) :
    discord_id {discord_id}, user_token {std::move(user_token)} {
    update();
}

User::User(long discord_id) : discord_id {discord_id} {
    bsoncxx::document::value document{DatabaseManager::fetch_user_document(discord_id)};

    user_id = {document.find("_id")->get_int64()};
    user_token = {document["token"].get_string().value.to_string()};
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

void User::update() {
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

    courses.clear();
    courses_stream >> courses_data;

    long term_id_of_most_recent = 0;

    for (const auto &course : courses_data) {

        if (!course.contains("id") || !course.contains("enrollment_term_id")) {
            continue;
        }

        if (course["id"].is_null() || course["enrollment_term_id"].is_null()) {
            continue;
        }

        long term_id {course["enrollment_term_id"]};
        if(term_id > term_id_of_most_recent) term_id_of_most_recent = term_id;
    }

    for (const auto &course : courses_data) {
        if (!course.contains("id") || !course.contains("enrollment_term_id")) {
            continue;
        }

        if (course["id"].is_null() || course["enrollment_term_id"].is_null()) {
            continue;
        }

        if(course["enrollment_term_id"] != term_id_of_most_recent) continue;

        courses.push_back(course["id"]);
    }
}

