#include <dpp/dpp.h>
#include "include/Course.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include "include/CanvasAPI.h"
#include "include/Guild.h"
#include <vector>

Course::Course(long course_id, const std::string &access_token) : course_id {course_id} {
    update(access_token);
}

Course::Course(long course_id) : course_id {course_id} {
    bsoncxx::document::value document{DatabaseManager::fetch_user_document(course_id)};

    name = document["name"].get_string().value.to_string();

    auto guild_array{document["tracking_guilds"]};
    bsoncxx::array::view guild_view = guild_array.get_array();

    for (const auto &guild_value: guild_view) {
        tracking_guilds.push_back(guild_value.get_int64());
    }
}

void Course::save() const {
    try {
        DatabaseManager::fetch_course_document(course_id);
    } catch (DocumentNotFoundException &ex) {
        DatabaseManager::insert_course(*this);
        return;
    }

    DatabaseManager::update_course(*this);
}

void Course::update(const std::string &override_token) {
    std::string accessor_token {override_token};
    if(accessor_token.empty()) accessor_token = {find_accessor().user_token};

    auto promise {CanvasAPI::get_course(course_id, accessor_token)};

    std::stringstream stream {promise->get_future().get()};
    nlohmann::json data;

    stream >> data;

    course_id = {data["id"]};
    name = {data["course_code"]};

    std::vector<Guild> guild_list {Guild::get_tracking_guilds(*this)};
    std::vector<long> guild_id_list {};

    tracking_guilds.clear();
    guild_id_list.reserve(guild_list.size());

    for (const Guild &guild : guild_list) {
        guild_id_list.push_back(guild.guild_id);
    }

    tracking_guilds = {guild_id_list};
}

User &Course::find_accessor() {
    for(const auto &guild_id: tracking_guilds) {
        Guild guild {Guild::get_guild(guild_id)};

        if (guild.tracked_courses.count(course_id) < 1) {
            guild.update();
            continue;
        }

        TrackedCourse tracked_course = *(guild.tracked_courses[course_id]);
        auto role {dpp::find_role(tracked_course.role_id)};

        auto discord_guild {dpp::find_guild(guild_id)};

        for (const auto &member: discord_guild->members) {
            int count = std::count(
                    member.second.roles.begin(),
                    member.second.roles.end(),
                    role->id
            );

            if (count > 0) {
                return User::get_user(static_cast<long>(member.first));
            }
        }

        if(!guild.is_tracking(*this)) continue;




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

