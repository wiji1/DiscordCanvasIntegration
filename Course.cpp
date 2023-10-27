#include <dpp/dpp.h>
#include "include/Course.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include "include/exceptions/AccessorNotFoundException.h"
#include "include/CanvasAPI.h"
#include "include/Guild.h"
#include "include/ImageFromHTML.h"
#include <vector>

std::unordered_map<long, std::shared_ptr<Course>> Course::course_map;

Course::Course(long course_id, const std::string &access_token) : course_id {course_id} {

}

Course::Course(long course_id) : course_id {course_id} {
    bsoncxx::document::value document{DatabaseManager::fetch_course_document(course_id)};

    name = document["name"].get_string().value.to_string();

    auto guild_array{document["tracking_guilds"]};
    bsoncxx::array::view guild_view = guild_array.get_array();

    for(const auto &guild_value: guild_view) {
        tracking_guilds.push_back(guild_value.get_int64());
    }

    auto assignment_array{document["recent_assignments"]};
    bsoncxx::array::view assignment_view = assignment_array.get_array();

    for(const auto &assignment_value: assignment_view) {
        recent_assignments.push_back(assignment_value.get_int64());
    }

    auto announcement_array{document["recent_announcements"]};
    bsoncxx::array::view announcement_view = announcement_array.get_array();

    for(const auto &announcement_value: announcement_view) {
        recent_announcements.push_back(announcement_value.get_int64());
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

dpp::task<void> Course::update(const std::string &override_token, bool override) {
    if((!is_active || is_updating) && !override) {
        std::cout << "Inactive!" << " " << is_updating << " " << is_active << std::endl;
        co_return;
    }

    is_updating = true;

    std::string accessor_token {override_token};
    try {
        //TODO: If user is invalid, request again without after updating the user, make a new exception for this
        //TODO: On user update, if key is invalid, un-verify them
        if(accessor_token.empty()) accessor_token = {find_accessor().user_token};
    } catch(AccessorNotFoundException &ex) {
        remove();
        std::cout << "Removing course: " << course_id << " " << is_updating << " " << is_active << std::endl;

        co_return;
    }

    std::string course_response = co_await CanvasAPI::get_course(course_id, accessor_token);
    nlohmann::json course_data;
    std::stringstream course_stream {course_response};
    course_stream >> course_data;

    course_id = {course_data["id"]};
    name = {course_data["course_code"]};

    try {
        std::string assignment_response = co_await CanvasAPI::get_assignments(course_id, accessor_token);
        std::stringstream assignment_stream{assignment_response};
        nlohmann::json assignment_data;
        assignment_stream >> assignment_data;

        for (const auto &assignment: assignment_data) {
            int id = {assignment["id"]};

            if (std::count(recent_assignments.begin(), recent_assignments.end(), id)) continue;
            //TODO: Post Assignment to channels
            std::cout << "Posting Assignment: " << id << std::endl;

            recent_assignments.push_back(id);
        }

        for(const auto &item: recent_assignments) {
            bool found{false};
            for (const auto &assignment: assignment_data) {
                int id = {assignment["id"]};
                if (item == id) {
                    found = true;
                    break;
                }
            }
            if (!found) std::remove(recent_assignments.begin(), recent_assignments.end(), item);
        }

        std::string announcement_response = co_await CanvasAPI::get_announcements(course_id, accessor_token);
        std::stringstream announcement_stream{announcement_response};
        nlohmann::json announcement_data;
        announcement_stream >> announcement_data;

        for(const auto &announcement: announcement_data) {
            int id = {announcement["id"]};

            if (std::count(recent_announcements.begin(), recent_announcements.end(), id)) continue;
            //TODO: Post announcements to channels
            for(const auto &guild_id: tracking_guilds) {
                Guild guild = *Guild::get_guild(guild_id);
                for(const auto &tracked_course: guild.tracked_courses) {

                    if(tracked_course->course_id != course_id) continue;

                    ImageFromHTML::post_announcement_embed(tracked_course->announcements_channel, announcement["message"],
               announcement["title"], announcement["url"], "Author");
                }
            }

            std::cout << "Posting Announcement: " << id << std::endl;

            recent_announcements.push_back(id);
        }

        for(const auto &item: recent_announcements) {
            bool found{false};
            for (const auto &announcement: announcement_data) {
                int id = {announcement["id"]};
                if (item == id) {
                    found = true;
                    break;
                }
            }
            if (!found) std::remove(recent_announcements.begin(), recent_announcements.end(), item);
        }
    } catch(nlohmann::detail::parse_error &ex) {
        std::cout << ex.what() << std::endl;
    }

    save();
    is_updating = false;

    //Changed this handling to happen within Guild object
//    std::vector<Guild> guild_list {Guild::get_tracking_guilds(*this)};
//    std::vector<long> guild_id_list {};
//
//    tracking_guilds.clear();
//    guild_id_list.reserve(guild_list.size());
//
//    for(const Guild &guild : guild_list) {
//        guild_id_list.push_back(guild.guild_id);
//    }
//
//    tracking_guilds = {guild_id_list};
}

User &Course::find_accessor() {
    for(const auto &guild_id: tracking_guilds) {
        Guild guild = *Guild::get_guild(guild_id);

        std::shared_ptr<TrackedCourse> tracked_course_ptr {nullptr};
        for(const auto &item: guild.tracked_courses) {
            if(item->course_id == course_id) tracked_course_ptr = item;
        }

        if(tracked_course_ptr == nullptr) {
            guild.update();
            continue;
        }

        TrackedCourse tracked_course = *tracked_course_ptr;
        auto role {dpp::find_role(tracked_course.role_id)};

        auto discord_guild {dpp::find_guild(guild_id)};

        for(const auto &member: discord_guild->members) {
            auto count = std::count(
                    member.second.roles.begin(),
                    member.second.roles.end(),
                    role->id
            );

            if(count > 0) {
                User &user {*User::get_user(static_cast<long>(member.first))};
                if(std::count(user.courses.begin(), user.courses.end(), course_id)) return user;
            }
        }
    }

    throw AccessorNotFoundException("An accessor for that course was not found!", course_id);
}


dpp::task<std::shared_ptr<Course>> Course::get_or_create(long course_id, const std::string &access_token, bool suspend) {
    try {
        co_return get_course(course_id);
    } catch(DocumentNotFoundException &ignored) { }

    std::cout << "Creating Course: " << course_id << std::endl;
    std::shared_ptr<Course> ptr = std::make_shared<Course>(course_id, access_token);
    if(suspend) {
        ptr->is_active = false;
    }
    co_await ptr->update(access_token, suspend);
    course_map[course_id] = ptr;

//    course_map[course_id] = std::make_unique<Course>(course_id, access_token);
    std::cout << "Created Course: " << course_id << std::endl;
    co_return course_map[course_id];
}

std::shared_ptr<Course> &Course::get_course(long course_id) {
    if(course_map.count(course_id)) return course_map[course_id];

    DatabaseManager::fetch_course_document(course_id);
    // Used to throw Exception if Course is not in the database

    course_map[course_id] = std::make_unique<Course>(course_id);
    return course_map[course_id];
}

void Course::remove() const {
    DatabaseManager::delete_course(*this);

    for(auto it = course_map.begin(); it != course_map.end();) {
        if (it->second.get() == this) {
            course_map.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

