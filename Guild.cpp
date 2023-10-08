#include <vector>
#include "include/Guild.h"
#include "include/DatabaseManager.h"
#include "include/exceptions/DocumentNotFoundException.h"

std::unordered_map<long, std::shared_ptr<Guild>> Guild::guild_map;

Guild::Guild(long guild_id) : guild_id {guild_id} {
    try {
        const bsoncxx::document::value document {DatabaseManager::fetch_guild_document(guild_id)};

        document_init(document);
    } catch(DocumentNotFoundException &ex) {
        //Initialize new Guild object

        create_verified_role();
    }
}

Guild::Guild(const bsoncxx::document::value& document) {
    document_init(document);
}

void Guild::document_init(const bsoncxx::document::value &document) {
    guild_id = {document.find("_id")->get_int64()};
    verified_role_id = {document.find("verified_role_id")->get_int64()};
    used_roles = {document.find("used_roles")->get_int32()};

    auto user_array{document["verified_users"]};
    bsoncxx::array::view user_view = user_array.get_array();

    for(const auto &user_value: user_view) {
        verified_users.push_back(user_value.get_int64());
    }

    auto tracked_courses_array = document["tracked_courses"];
    bsoncxx::array::view tracked_courses_view = tracked_courses_array.get_array();

    for(const auto &course_value : tracked_courses_view) {
        bsoncxx::document::view course_doc_view = course_value.get_document();

        long course_id = course_doc_view["course_id"].get_int64();
        long category_id = course_doc_view["category_id"].get_int64();
        long announcements_channel = course_doc_view["announcements_channel"].get_int64();
        long forums_channel = course_doc_view["forums_channel"].get_int64();
        long role_id = course_doc_view["role_id"].get_int64();

        tracked_courses.emplace_back(std::make_shared<TrackedCourse>(course_id, category_id, announcements_channel, forums_channel, role_id));
    }
}

dpp::task<void> Guild::add_tracked_course(long course_id) {

    if(used_roles >= MAXIMUM_ROLE_REQUESTS) {
        std::cerr << "Failed to track course " << course_id << " for guild " << guild_id << " due to not enough remaining API requests." << std::endl;
        co_return;
    }

    used_roles++;

    //TODO: Verify Role/Channel existence before performing any actions

    Course &course {*Course::get_course(course_id)};
    course.tracking_guilds.push_back(guild_id);
    course.save();

    std::shared_ptr<TrackedCourse> tracked_course = std::make_shared<TrackedCourse>();
    tracked_course->course_id = course_id;

    //Create Course Role
    dpp::role course_role;
    course_role.guild_id = guild_id;
    course_role.set_name(course.name);

    dpp::confirmation_callback_t role_callback = co_await bot->co_role_create(course_role);
    if(role_callback.http_info.ratelimit_remaining == role_callback.http_info.ratelimit_limit - 1) used_roles = 0;

    if(role_callback.is_error()) {
        std::cout << "Error: " << role_callback.get_error().message << std::endl;
    }

    course_role = std::get<dpp::role>(role_callback.value);
    tracked_course->role_id = static_cast<long>(course_role.id);

    for(const auto &item: verified_users) {
        User &user {*User::get_user(item)};
        if(!std::count(user.courses.begin(), user.courses.end(), course_id)) continue;
        bot->guild_member_add_role(guild_id, user.discord_id, course_role.id);
    }

    //Create Category Channel
    dpp::channel category {};
    category.set_guild_id(guild_id);
    category.set_type(dpp::channel_type::CHANNEL_CATEGORY);
    category.set_name(course.name);

    dpp::permission category_allow {};
    dpp::permission category_deny {};

    category_allow.set(dpp::p_view_channel);
    category.add_permission_overwrite(course_role.id, dpp::overwrite_type::ot_role, category_allow, category_deny);

    category_allow.set();
    category_deny.set(dpp::p_view_channel);

    category.add_permission_overwrite(guild_id, dpp::overwrite_type::ot_role, category_allow, category_deny);

    dpp::confirmation_callback_t category_callback = co_await bot->co_channel_create(category);
    if(category_callback.is_error()) {
        std::cout << "Error: " << category_callback.get_error().message << std::endl;
    }
    category = std::get<dpp::channel>(category_callback.value);
    tracked_course->category_id = static_cast<long>(category.id);

    //Create Announcements Channel
    dpp::channel announcements {};
    announcements.set_guild_id(guild_id);
    announcements.set_type(dpp::channel_type::CHANNEL_TEXT);
    announcements.set_parent_id(category.id);
    announcements.set_name("Announcements");
    announcements.set_position(1);

    dpp::permission announcements_allow {};
    dpp::permission announcements_deny {};

    announcements_deny.set(dpp::p_send_messages);
    announcements.add_permission_overwrite(course_role.id, dpp::overwrite_type::ot_role, announcements_allow, announcements_deny);

    dpp::confirmation_callback_t announcements_callback = co_await bot->co_channel_create(announcements);
    if(announcements_callback.is_error()) {
        std::cout << "Error: " << announcements_callback.get_error().message << std::endl;
    }
    announcements = std::get<dpp::channel>(announcements_callback.value);
    tracked_course->announcements_channel = static_cast<long>(announcements.id);

    //Create Forums Channel
    dpp::channel forums {};
    forums.set_guild_id(guild_id);
    forums.set_type(dpp::channel_type::CHANNEL_FORUM);
    forums.set_parent_id(category.id);
    forums.set_name("Assignments");
    forums.set_position(1);
    forums.set_default_forum_layout(dpp::forum_layout_type::fl_list_view);

    dpp::permission forums_allow {};
    dpp::permission forums_deny {};

    forums_allow.set(dpp::p_send_messages_in_threads);
    forums_deny.set(dpp::p_manage_threads, dpp::p_create_public_threads, dpp::p_create_private_threads, dpp::p_send_messages);
    forums.add_permission_overwrite(course_role.id, dpp::overwrite_type::ot_role, forums_allow, forums_deny);

    dpp::confirmation_callback_t forums_callback = co_await bot->co_channel_create(forums);
    if(forums_callback.is_error()) {
        std::cout << "Error: " << forums_callback.get_error().message << std::endl;
    }
    forums = std::get<dpp::channel>(forums_callback.value);
    tracked_course->forums_channel = static_cast<long>(forums.id);

    tracked_courses.push_back(tracked_course);
    std::cout << "Tracked Courses Size: " << tracked_courses.size() << std::endl;
}

void Guild::remove_tracked_course(const std::shared_ptr<TrackedCourse> &tracked_course) {
    bot->role_delete(guild_id, tracked_course->role_id);
    bot->channel_delete(tracked_course->category_id);
    bot->channel_delete(tracked_course->forums_channel);
    bot->channel_delete(tracked_course->announcements_channel);

    Course &course_object {*Course::get_course(tracked_course->course_id)};
    std::vector<long> &guilds {course_object.tracking_guilds};
    guilds.erase(std::remove(guilds.begin(), guilds.end(), guild_id), guilds.end());
    course_object.save();

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

dpp::task<void> Guild::update() {
    std::vector<long> to_add{};

    std::vector<std::shared_ptr<TrackedCourse>> active_courses{tracked_courses};
    std::vector<long> active_users{verified_users};

    for(const auto &user_id: active_users) {
        std::cout << "Looking at User: " << user_id << std::endl;
        std::shared_ptr<User> user{nullptr};

        try {
            user = User::get_user(user_id);
        } catch (const DocumentNotFoundException &ex) {
            verified_users.erase(std::remove(verified_users.begin(), verified_users.end(), user_id),
                                 verified_users.end());

            continue;
        }

        std::vector<long> user_courses{user->courses};
        for(const auto &course: user_courses) {
            std::cout << "Looking at course: " << course << std::endl;
            try { Course::get_course(course); } catch (DocumentNotFoundException &ex) {
                [user, this]() -> dpp::job {co_await update_user(*user, false);}();
                co_return;
            }

            bool course_found = false;

            for(auto it = active_courses.begin(); it != active_courses.end();) {
                const auto &tracked_course = *it;

                if(tracked_course->course_id == course) {
                    it = active_courses.erase(it);

                    Course &course_object{*Course::get_course(course)};
                    if(std::count(course_object.tracking_guilds.begin(), course_object.tracking_guilds.end(),
                                   guild_id) < 1) {
                        course_object.tracking_guilds.push_back(guild_id);
                        course_object.save();
                    }

                    course_found = true;
                } else {
                    ++it;
                }
            }

            if(!course_found) {
                std::cout << "Adding course " << course << " to to_add list" << std::endl;
                to_add.push_back(course);
            }
        }
    }

    for(const auto &tracked_course: active_courses) {
        remove_tracked_course(tracked_course);
    }

    std::vector<dpp::task<void>> tasks;

    tasks.reserve(to_add.size());
    for(const auto &course : to_add) {
        tasks.emplace_back(this->add_tracked_course(course));
    }

    for(auto &task: tasks) co_await task;

    //TODO: Possibly move save method outside of verify_existence
    //TODO: Ensure that guild cannot be deregistered during verification (Coroutines will have to be canceled, look for another method)

    //TODO: Filter forum response
    co_await verify_existence();

    for(const auto &course_id: to_add) {
        Course &course {*Course::get_course(course_id)};
        course.is_active = true;
    }
}

dpp::task<void> Guild::update_user(User &user, bool verify) {
    std::cout << "Verify status: " << verify << std::endl;
    co_await user.update(true);
    std::cout << "Updating user1" << std::endl;
    if(verify) co_await verify_user(user.discord_id, false);
    else co_await update();
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

std::shared_ptr<Guild> &Guild::get_guild(long guild_id) {
    return guild_map[guild_id];
}

void Guild::deregister() {
    std::cout << "Desregistering" << std::endl;
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

dpp::task<void> Guild::verify_existence() {
    std::cout << "Verifying existence" << std::endl;

    dpp::confirmation_callback_t roles_callback = co_await bot->co_roles_get(guild_id);
    if(roles_callback.is_error()) {
        std::cout << "de1" << std::endl;
        deregister();
        co_return;
    }

    dpp::role_map role_map = std::get<dpp::role_map>(roles_callback.value);

    if(role_map.find(verified_role_id) == role_map.end()) {
        std::cout << "de2" << std::endl;
        deregister();
        co_return;
    }

    auto active_courses {tracked_courses};

    for(const auto &course: active_courses) {
        if(!course->verify_role_existence(role_map)) remove_tracked_course(course);
    }

    dpp::confirmation_callback_t channels_callback = co_await bot->co_channels_get(guild_id);
    dpp::channel_map channel_map = std::get<dpp::channel_map>(channels_callback.value);

    for(const auto &course: active_courses) {
        if(!course->verify_channel_existence(channel_map)) remove_tracked_course(course);
    }

    std::cout << "Saving: " << tracked_courses.size() << std::endl;
    save();
}

dpp::task<void> Guild::verify_user(long user_id, bool create) {
    User &user {*User::get_user(user_id)};
    if(create) co_await user.update();
    //TODO: Potential double update

    for(const auto &course_id: user.courses) {
        bool fail = false;
        try {
            Course &course {*Course::get_course(course_id)};
            course.is_active = false;
        } catch(DocumentNotFoundException &ex) {
          fail = true;
        }

        if(fail) {
            co_await update_user(user, true);
            co_return;
        }
    }

    verified_users.push_back(user_id);
    std::cout << "User courses size: " << user.courses.size() << std::endl;
    co_await update();

    for(const auto &course_id: user.courses) {
        for(const auto &tracked_course: tracked_courses) {
            if(tracked_course->course_id != course_id) continue;

            bot->guild_member_add_role(guild_id, user_id, tracked_course->role_id, [&](auto &callback) {
                Course &course {*Course::get_course(course_id)};
                course.is_active = true;
            });
        }
    }
}

