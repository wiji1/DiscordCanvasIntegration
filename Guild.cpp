#include <vector>
#include "include/Guild.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"

std::unordered_map<long, std::shared_ptr<Guild>> Guild::guild_map;

//TODO: Add a cleanup method to make testing easier (Delete roles and channels through remove_tracked_courses method)

Guild::Guild(long guild_id) : guild_id {guild_id} {
    std::cout << "Standard Constructor" << std::endl;
    try {
        const bsoncxx::document::value document {DatabaseManager::fetch_guild_document(guild_id)};

        document_init(document);
    } catch(DocumentNotFoundException &ex) {
        //Initialize new Guild object

        create_verified_role();
    }
}

Guild::Guild(bsoncxx::document::value document) {
    std::cout << "Document Constructor" << std::endl;
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

    std::shared_ptr<TrackedCourse> tracked_course = std::make_shared<TrackedCourse>();
    std::cout << "Pre Course ID: " << course_id << &(*tracked_course) << std::endl;
    tracked_course->course_id = course_id;

    int completedCallbacks = 0;
    std::condition_variable cv;
    std::mutex mtx;

    auto handleCallback = [&completedCallbacks, &cv, &tracked_course, &mtx, this](bool isError) {
        std::unique_lock<std::mutex> lock(mtx);
        if (isError) {
            std::cout << "Error occurred in callback." << std::endl;
        }

        completedCallbacks++;
        if(completedCallbacks == 4) {
            tracked_courses.push_back(tracked_course);

            cv.notify_all();
        }
    };

    dpp::channel category {};
    category.set_guild_id(guild_id);
    category.set_type(dpp::channel_type::CHANNEL_CATEGORY);
    category.set_name(course.name);

    bot->channel_create(category, [&tracked_course, handleCallback](auto& callback) {
        if (callback.is_error()) {
            std::cout << "Error: " << callback.get_error().message << std::endl;
        }

        dpp::channel category = std::get<dpp::channel>(callback.value);
        tracked_course->category_id = static_cast<long>(category.id);

        handleCallback(callback.is_error());
    });

    dpp::channel forums {};
    forums.set_guild_id(guild_id);
    forums.set_type(dpp::channel_type::CHANNEL_FORUM);
    forums.set_parent_id(category.id);
    forums.set_name("Assignments");

    bot->channel_create(forums, [&tracked_course, handleCallback](auto& callback) {
        if (callback.is_error()) {
            std::cout << "Error: " << callback.get_error().message << std::endl;
        }

        dpp::channel forums = std::get<dpp::channel>(callback.value);
        tracked_course->forums_channel = static_cast<long>(forums.id);

        handleCallback(callback.is_error());
    });

    dpp::channel announcements {};
    announcements.set_guild_id(guild_id);
    announcements.set_type(dpp::channel_type::CHANNEL_ANNOUNCEMENT);
    announcements.set_parent_id(category.id);
    announcements.set_name("Announcements");

    bot->channel_create(announcements, [&tracked_course, handleCallback](auto& callback) {
        if (callback.is_error()) {
            std::cout << "Error: " << callback.get_error().message << std::endl;
        }

        dpp::channel announcements = std::get<dpp::channel>(callback.value);
        tracked_course->announcements_channel = static_cast<long>(announcements.id);

        handleCallback(callback.is_error());
    });

    dpp::role course_role;
    course_role.guild_id = guild_id;
    course_role.set_name(course.name);

    bot->role_create(course_role, [&tracked_course, handleCallback](auto& callback) {
        if (callback.is_error()) {
            std::cout << "Error: " << callback.get_error().message << std::endl;
        }

        dpp::role role = std::get<dpp::role>(callback.value);
        tracked_course->role_id = static_cast<long>(role.id);

        handleCallback(callback.is_error());
    });

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&completedCallbacks]() { return completedCallbacks == 4; });
}

void Guild::remove_tracked_course(const std::shared_ptr<TrackedCourse>& tracked_course) {
    bot->role_delete(guild_id, tracked_course->role_id);
    bot->channel_delete(tracked_course->category_id);
    bot->channel_delete(tracked_course->forums_channel);
    bot->channel_delete(tracked_course->announcements_channel);

    tracked_courses.erase(std::remove(tracked_courses.begin(), tracked_courses.end(),
                                     tracked_course), tracked_courses.end());

//TODO: This is the issue, concurrent modification
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

    std::cout << 1 << std::endl;

    std::vector<std::shared_ptr<TrackedCourse>> active_courses {};
    std::push_heap(tracked_courses.begin(), tracked_courses.end());

    std::vector<long> to_add {};

    std::cout << 2 << std::endl;

    for(const auto &user_id: verified_users) {
        User user {User::get_user(user_id)};

        std::cout << "User: " << user_id << std::endl;

        main:
        for(const auto &course: user.courses) {
            std::cout << "Course: " << course << std::endl;
            for(const auto &tracked_course: tracked_courses) {
                std::cout << "Tracked: " << tracked_course->course_id << std::endl;
                if(tracked_course->course_id == course) {
                    std::cout << "Erasing active course" << std::endl;
                    active_courses.erase(std::remove(active_courses.begin(), active_courses.end(),
                                                     tracked_course), active_courses.end());
                    goto main;
                }
            }
            to_add.push_back(course);
        }
    }

    for(const auto &course: to_add) add_tracked_course(course);

    std::cout << 3 << std::endl;

//    for(const auto &course: active_courses) remove_tracked_course(course);


    std::cout << 4 << std::endl;
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
        save();
    });
}

void Guild::register_guild(long guild_id) {
    guild_map[guild_id] = std::make_unique<Guild>(guild_id);
    guild_map[guild_id]->save();
}

std::shared_ptr<Guild> Guild::get_guild(long guild_id) {
    std::cout << "Fetching!" << std::endl;
    for (const auto &item: guild_map) {
        std::cout << "Guild ID: " << item.first << std::endl;
    }

    std::cout << "Pre-Pass Tracked Courses size: " << guild_map[guild_id]->tracked_courses.size() << std::endl;
    return guild_map[guild_id];
}

bool Guild::is_tracking(const Course &course) {
    for(const auto &tracked_course : tracked_courses) {
        if(tracked_course->course_id == course.course_id) {
            return true;
        }
    }

    return false;
}

bool Guild::is_registered(long guild_id) {
    if(guild_map.count(guild_id)) return true;

    try {
        DatabaseManager::fetch_guild_document(guild_id);
    } catch(DocumentNotFoundException &ex) {
        return false;
    }

    return true;
}

