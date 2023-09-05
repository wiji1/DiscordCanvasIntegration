#include <vector>
#include "include/Guild.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"

std::unordered_map<long, std::unique_ptr<Guild>> Guild::guild_map;


Guild::Guild(long guild_id) : guild_id {guild_id} {
    try {
        const bsoncxx::document::value document {DatabaseManager::fetch_guild_document(guild_id)};

        document_init(document);
    } catch(DocumentNotFoundException &ex) {
        //Initialize new Guild object

        create_verified_role();
    }
}

Guild::Guild(const bsoncxx::document::value &document) {
    document_init(document);
}

void Guild::document_init(const bsoncxx::document::value &document) {
    guild_id = {document.find("_id")->get_int64()};
    verified_role_id = {document.find("verified_role_id")->get_int64()};

    auto user_array{document["verified_users"]};
    bsoncxx::array::view user_view = user_array.get_array();

    for(const auto &user_value: user_view) {
        verified_users.push_back(user_value.get_int64());
    }

    auto tracked_courses_array = document["tracked_courses"];
    bsoncxx::array::view tracked_courses_view = tracked_courses_array.get_array();

    for (const auto &course_value : tracked_courses_view) {
        bsoncxx::document::view course_doc_view = course_value.get_document();

        long course_id = course_doc_view["course_id"].get_int64();
        long category_id = course_doc_view["category_id"].get_int64();
        long announcements_channel = course_doc_view["announcements_channel"].get_int64();
        long forums_channel = course_doc_view["forums_channel"].get_int64();
        long role_id = course_doc_view["role_id"].get_int64();

        tracked_courses.emplace_back(std::make_shared<TrackedCourse>(course_id, category_id, announcements_channel, forums_channel, role_id));
    }
}

void Guild::add_tracked_course(long course_id) {
    Course course {Course::get_course(course_id)};
    std::shared_ptr<TrackedCourse> tracked_course {tracked_courses.emplace_back()};

    dpp::role course_role;
    course_role.guild_id = guild_id;
    course_role.set_name(course.name);

    bot->role_create(course_role, [&tracked_course](auto& callback) {
        if(callback.is_error()) return;

        dpp::role role = std::get<dpp::role>(callback.value);
        tracked_course->role_id = static_cast<long>(role.id);
    });

    dpp::channel category {};
    category.set_guild_id(guild_id);
    category.set_type(dpp::channel_type::CHANNEL_CATEGORY);
    category.set_name(course.name);

    bot->channel_create(category, [&tracked_course](auto& callback) {

        dpp::channel category = std::get<dpp::channel>(callback.value);
        tracked_course->category_id = static_cast<long>(category.id);
    });

    dpp::channel forums {};
    forums.set_guild_id(guild_id);
    forums.set_type(dpp::channel_type::CHANNEL_FORUM);
    forums.set_name("Assignments");

    bot->channel_create(forums, [&tracked_course](auto& callback) {

        dpp::channel forums = std::get<dpp::channel>(callback.value);
        tracked_course->forums_channel = static_cast<long>(forums.id);
    });

    dpp::channel announcements {};
    announcements.set_guild_id(guild_id);
    announcements.set_type(dpp::channel_type::CHANNEL_ANNOUNCEMENT);
    announcements.set_name("Announcements");

    bot->channel_create(announcements, [&tracked_course](auto& callback) {

        dpp::channel announcements = std::get<dpp::channel>(callback.value);
        tracked_course->announcements_channel = static_cast<long>(announcements.id);
    });
}

void Guild::remove_tracked_course(const std::shared_ptr<TrackedCourse>& tracked_course) {
    bot->role_delete(guild_id, tracked_course->role_id);
    bot->channel_delete(tracked_course->forums_channel);
    bot->channel_delete(tracked_course->announcements_channel);

    tracked_courses.erase(std::remove(tracked_courses.begin(), tracked_courses.end(),
                                     tracked_course), tracked_courses.end());
}

std::vector<Guild> Guild::get_tracking_guilds(Course &course) {
    std::vector<Guild> guildsTrackingCourse;

    for(const auto &guild_pair : guild_map) {
        Guild guild = *guild_pair.second;

        if(guild.is_tracking(course)) guildsTrackingCourse.push_back(guild);
    }
    return guildsTrackingCourse;
}

void Guild::update() {
    //Go through all verified users, load new tracked courses, and delete old ones.
    std::vector<std::shared_ptr<TrackedCourse>> active_courses {};
    std::push_heap(tracked_courses.begin(), tracked_courses.end());


    for(const auto &user_id: verified_users) {
        User user {User::get_user(user_id)};
        user.update();

        main:
        for(const auto &course: user.courses) {
            for(const auto &tracked_course: tracked_courses) {
                if(tracked_course->course_id == course) {
                    active_courses.erase(std::remove(active_courses.begin(), active_courses.end(),
                                                     tracked_course), active_courses.end());
                    goto main;
                }
            }
            add_tracked_course(course);
        }
    }

    for(const auto &course: active_courses) remove_tracked_course(course);
}

void Guild::save() const {
    try {
        DatabaseManager::fetch_guild_document(guild_id);
    } catch (DocumentNotFoundException &ex) {
        DatabaseManager::insert_guild(*this);
        return;
    }

    DatabaseManager::update_guild(*this);
}

void Guild::create_verified_role() {
    dpp::role verified_role;
    verified_role.guild_id = guild_id;
    verified_role.set_name("Verified");

    bot->role_create(verified_role, [this](auto& callback) {
        if(callback.is_error()) return;

        dpp::role role = std::get<dpp::role>(callback.value);
        this->verified_role_id = static_cast<long>(role.id);
    });
}

void Guild::register_guild(long guild_id) {
    guild_map[guild_id] = std::make_unique<Guild>(guild_id);
}

Guild Guild::get_guild(long guild_id) {
    return *guild_map[guild_id];
}

bool Guild::is_tracking(const Course &course) {
    for(const auto &tracked_course : tracked_courses) {
        if(tracked_course->course_id == course.course_id) {
            return true;
        }
    }

    return false;
}

