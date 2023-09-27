#include "include/User.h"
#include "include/CanvasAPI.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include <json.h>
#include <sstream>
#include <utility>

std::unordered_map<long, std::shared_ptr<User>> User::user_map;

User::User(std::string user_token, long discord_id) :
    discord_id {discord_id}, user_token {std::move(user_token)} {
}

User::User(long discord_id) : discord_id {discord_id} {
    bsoncxx::document::value document {DatabaseManager::fetch_user_document(discord_id)};

    user_id = {document.find("_id")->get_int64()};
    user_token = {document["token"].get_string().value.to_string()};
    name = document["name"].get_string().value.to_string();

    auto course_array{document["courses"]};
    bsoncxx::array::view course_view = course_array.get_array();

    for(const auto &course_value: course_view) {
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

dpp::task<void> User::update() {
    std::string user_response = co_await CanvasAPI::get_user_profile(user_token);
    std::stringstream profile_stream {user_response};
    nlohmann::json profile_data;

    profile_stream >> profile_data;

    this->user_token = {user_token};
    user_id = {profile_data["id"]};
    name = {profile_data["name"]};

    std::string courses_response = co_await CanvasAPI::get_user_courses(user_token);
    std::stringstream courses_stream {courses_response};
    nlohmann::json courses_data;

    courses.clear();
    courses_stream >> courses_data;

    long term_id_of_most_recent = 0;

    for(const auto &course : courses_data) {

        if (!course.contains("id") || !course.contains("enrollment_term_id")) {
            continue;
        }

        if (course["id"].is_null() || course["enrollment_term_id"].is_null()) {
            continue;
        }

        long term_id {course["enrollment_term_id"]};
        if(term_id > term_id_of_most_recent) term_id_of_most_recent = term_id;
    }

    std::vector<dpp::task<void>> tasks;
    tasks.reserve(courses_data.size());
    std::cout << "Course Data size: " << courses_data.size() << std::endl;

    for(const auto &course : courses_data) {
        if (!course.contains("id") || !course.contains("enrollment_term_id")) {
            continue;
        }

        if (course["id"].is_null() || course["enrollment_term_id"].is_null()) {
            continue;
        }

        if(course["enrollment_term_id"] != term_id_of_most_recent) continue;


        std::cout << course["name"] << " " << course["enrollment_term_id"] << std::endl;

        courses.push_back(course["id"]);
        auto course_obj = co_await Course::get_or_create(course["id"], user_token);
        tasks.emplace_back(course_obj->update(user_token));
    }
    for(auto &task: tasks) co_await task;

    std::cout << "Saving User: " << name << std::endl;
    save();
}

std::shared_ptr<User> &User::create_user(std::string user_token, long discord_id) {
    user_map[discord_id] = std::make_unique<User>(std::move(user_token), discord_id);
    return user_map[discord_id];
}

std::shared_ptr<User> &User::get_user(long discord_id) {
    if(user_map.count(discord_id)) return user_map[discord_id];

    DatabaseManager::fetch_user_document(discord_id);
    // Used to throw Exception if User is not in the database

    user_map[discord_id] = std::make_unique<User>(discord_id);
    return user_map[discord_id];
}


