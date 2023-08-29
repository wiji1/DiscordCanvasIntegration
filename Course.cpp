#include <dpp/dpp.h>
#include "include/Course.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include "include/CanvasAPI.h"

Course::Course(long course_id) : course_id {course_id} {

}

Course::Course(long course_id, const std::string &access_token) {

}

void Course::save() const {

}

void Course::update() {
    User accessor {find_accessor()};

    auto promise {CanvasAPI::get_course(course_id, accessor.user_token)};

    std::stringstream stream {promise->get_future().get()};
    nlohmann::json data;

    stream >> data;

    course_id = {data["id"]};
    name = {data["course_code"]};

    //TODO: Loop through all guilds and search for tracked courses
}

User &Course::find_accessor() {
    for(const auto &guild_id: trackingGuilds) {
        for(const auto &item: dpp::find_guild(guild_id)->members) {

        }
    }
    return User::get_user(0);
}


Course &Course::get_or_create(long course_id, const std::string &access_token) {
    try {
        return get_course(course_id);
    } catch(DocumentNotFoundException &ignored) { }

    course_map[course_id] = std::make_unique<Course>(course_id, access_token);
    return *course_map[course_id];
}

Course &Course::get_course(long course_id) {
    if(course_map.count(course_id)) return *course_map[course_id];

    DatabaseManager::fetch_course_document(course_id);
    // Used to throw Exception if Course is not in the database

    course_map[course_id] = std::make_unique<Course>(course_id);
    return *course_map[course_id];
}

