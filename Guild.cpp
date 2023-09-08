#include <vector>
#include "include/Guild.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"

std::unordered_map<long, std::shared_ptr<Guild>> Guild::guild_map;

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

    //TODO: Find out what's rate limiting me in here. Also add in rate limit handling (Discord role creation API, investigate)
    //TODO: Add startup channel/role existence verification. Properly remove tracked course/guild when not found
    //TODO: Implement course updating

    Course course {Course::get_course(course_id)};

    std::shared_ptr<TrackedCourse> tracked_course = std::make_shared<TrackedCourse>();
    tracked_course->course_id = course_id;

    int completedCallbacks = 0;
    std::condition_variable cv;
    std::mutex mtx;

    auto handle_callback = [&completedCallbacks, &cv, &tracked_course, &mtx, this](bool isError) {
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

    std::promise<dpp::role> role_promise;
    std::future<dpp::role> role_future = role_promise.get_future();

    dpp::role course_role;
    course_role.guild_id = guild_id;
    course_role.set_name(course.name);

    bot->role_create(course_role, [&](auto &callback) {
        if (callback.is_error()) {
            std::cout << "Error: " << callback.get_error().message << std::endl;
        }

        dpp::role role = std::get<dpp::role>(callback.value);
        tracked_course->role_id = static_cast<long>(role.id);

        role_promise.set_value(role);
        handle_callback(callback.is_error());
    });

    dpp::role role = role_future.get();

    std::promise<long> category_promise;
    std::future<long> category_future = category_promise.get_future();

    dpp::channel category {};
    category.set_guild_id(guild_id);
    category.set_type(dpp::channel_type::CHANNEL_CATEGORY);
    category.set_name(course.name);

    dpp::permission category_allow {};
    dpp::permission category_deny {};

    category_allow.set(dpp::p_view_channel);
    category.add_permission_overwrite(role.id, dpp::overwrite_type::ot_role, category_allow, category_deny);

    category_allow.set();
    category_deny.set(dpp::p_view_channel);

    category.add_permission_overwrite(guild_id, dpp::overwrite_type::ot_role, category_allow, category_deny);



    bot->channel_create(category, [&](auto &callback) {
        if (callback.is_error()) {
            std::cout << "Error: " << callback.get_error().message << std::endl;
        }

        dpp::channel category = std::get<dpp::channel>(callback.value);
        tracked_course->category_id = static_cast<long>(category.id);

        category_promise.set_value(static_cast<long>(category.id));
        handle_callback(callback.is_error());
    });

    long categoryId = category_future.get();

    dpp::channel announcements {};
    announcements.set_guild_id(guild_id);
    announcements.set_type(dpp::channel_type::CHANNEL_TEXT);
    announcements.set_parent_id(categoryId);
    announcements.set_name("Announcements");
    announcements.set_position(1);

    dpp::permission announcements_allow {};
    dpp::permission announcements_deny {};

    announcements_deny.set(dpp::p_send_messages);
    announcements.add_permission_overwrite(role.id, dpp::overwrite_type::ot_role, announcements_allow, announcements_deny);

    bot->channel_create(announcements, [&](auto &callback) {
        if (callback.is_error()) {
            std::cout << "Error: " << callback.get_error().message << std::endl;
        }

        dpp::channel announcements = std::get<dpp::channel>(callback.value);
        tracked_course->announcements_channel = static_cast<long>(announcements.id);

        handle_callback(callback.is_error());

        dpp::channel forums {};
        forums.set_guild_id(guild_id);
        forums.set_type(dpp::channel_type::CHANNEL_FORUM);
        forums.set_parent_id(categoryId);
        forums.set_name("Assignments");
        forums.set_position(1);
        forums.set_default_forum_layout(dpp::forum_layout_type::fl_list_view);

        dpp::permission forums_allow {};
        dpp::permission forums_deny {};

        forums_allow.set(dpp::p_send_messages_in_threads);
        forums_deny.set(dpp::p_manage_threads, dpp::p_create_public_threads, dpp::p_create_private_threads);
        forums.add_permission_overwrite(role.id, dpp::overwrite_type::ot_role, forums_allow, forums_deny);

        bot->channel_create(forums, [&](auto &callback) {
            if (callback.is_error()) {
                std::cout << "Error: " << callback.get_error().message << std::endl;
            }

            dpp::channel forums = std::get<dpp::channel>(callback.value);
            tracked_course->forums_channel = static_cast<long>(forums.id);

            handle_callback(callback.is_error());
        });
    });

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&completedCallbacks]() { return completedCallbacks == 4; });
}

void Guild::remove_tracked_course(const std::shared_ptr<TrackedCourse>& tracked_course) {
    bot->role_delete(guild_id, tracked_course->role_id);
    bot->channel_delete(tracked_course->category_id);
    bot->channel_delete(tracked_course->forums_channel);
    bot->channel_delete(tracked_course->announcements_channel);

    for(int i {0}; i < tracked_courses.size(); i++) {
        TrackedCourse &course = *tracked_courses[i];
        if(course.course_id == tracked_course->course_id) {
            tracked_courses.erase(tracked_courses.begin() + i);
            break;
        }
    }
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
    std::vector<long> to_add {};

    std::vector<std::shared_ptr<TrackedCourse>> active_courses {tracked_courses};
    std::vector<long> active_users {verified_users};

    for(const auto &user_id : active_users) {
        std::unique_ptr<User> user {nullptr};

        try {
            user = std::make_unique<User>(User::get_user(user_id));
        } catch (const DocumentNotFoundException &ex) {
            verified_users.erase(std::remove(verified_users.begin(), verified_users.end(), user_id), verified_users.end());

            continue;
        }

        for(const auto &course : user->courses) {
            bool course_found = false;

            for(auto it = active_courses.begin(); it != active_courses.end();) {
                const auto &tracked_course = *it;

                if (tracked_course->course_id == course) {
                    it = active_courses.erase(it);
                    course_found = true;
                } else {
                    ++it;
                }
            }

            if (!course_found) {
                to_add.push_back(course);
            }
        }
    }

    for (const auto &tracked_course : active_courses) {
        remove_tracked_course(tracked_course);
    }

    for(const auto &course: to_add) add_tracked_course(course);

    save();
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
    return guild_map[guild_id];
}

void Guild::deregister() {
    auto active_courses {tracked_courses};
    for(const auto &course: active_courses) {
        remove_tracked_course(course);
    }

    bot->role_delete(guild_id, verified_role_id);

    DatabaseManager::delete_guild(*this);

    for(auto it = guild_map.begin(); it != guild_map.end();) {
        if (it->second.get() == this) {
            guild_map.erase(it);
            break;
        } else {
            ++it;
        }
    }
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

