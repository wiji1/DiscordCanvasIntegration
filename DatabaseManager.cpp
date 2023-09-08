#include "include/DatabaseManager.h"
#include "include/ConfigManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include "include/Guild.h"
#include <optional>
#include <exception>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

void DatabaseManager::init() {
    mongocxx::instance instance {};

    if(!ConfigManager::data.contains("mongo-uri")) throw std::runtime_error {"Config file does not contain mongo uri"};

    std::string uri_string {ConfigManager::data["mongo-uri"]};
    mongocxx::uri uri {uri_string};

    mongocxx::options::client client_options;
    const auto api = mongocxx::options::server_api{mongocxx::options::server_api::version::k_version_1};
    client_options.server_api_opts(api);

    client = {uri, client_options};

    database = client["canvas"];
    user_collection = database["users"];
    course_collection = database["courses"];
    guild_collection = database["guilds"];

    load_guilds();
}

void DatabaseManager::load_guilds() {
    mongocxx::cursor cursor = guild_collection.find(bsoncxx::document::view_or_value{});

    for(auto&& doc : cursor) {
        bsoncxx::document::value docValue{doc};

        long id = docValue.find("_id")->get_int64();
        Guild::guild_map[id] = std::make_unique<Guild>(docValue);
        Guild::guild_map[id]->update();
    }

}

bsoncxx::document::value DatabaseManager::fetch_user_document(long id) {
    auto find_one_filtered_result = user_collection.find_one(make_document(kvp("discord_id", bsoncxx::types::b_int64{id})));
    if (find_one_filtered_result) {
        bsoncxx::document::value doc = find_one_filtered_result.value();
        return doc;
    } else {
        std::stringstream long_stream {};
        long_stream >> id;

        throw DocumentNotFoundException {"Document with id " + long_stream.str() + " was not found!"};
    }
    assert(find_one_filtered_result);
}

bsoncxx::document::value DatabaseManager::fetch_course_document(long id) {
    auto find_one_filtered_result = course_collection.find_one(make_document(kvp("_id", bsoncxx::types::b_int64{id})));
    if (find_one_filtered_result) {
        bsoncxx::document::value doc = find_one_filtered_result.value();
        return doc;
    } else {
        std::stringstream long_stream {};
        long_stream >> id;

        throw DocumentNotFoundException {"Document with id " + long_stream.str() + " was not found!"};
    }
    assert(find_one_filtered_result);
}

bsoncxx::document::value DatabaseManager::fetch_guild_document(long id) {
    auto find_one_filtered_result = guild_collection.find_one(make_document(kvp("_id", bsoncxx::types::b_int64{id})));
    if (find_one_filtered_result) {
        bsoncxx::document::value doc = find_one_filtered_result.value();
        return doc;
    } else {
        std::stringstream long_stream {};
        long_stream >> id;

        throw DocumentNotFoundException {"Document with id " + long_stream.str() + " was not found!"};
    }
    assert(find_one_filtered_result);
}

void DatabaseManager::update_user(const User &user) {
    bsoncxx::builder::basic::array courses_array_builder;
    for (const long &course : user.courses) {
        courses_array_builder.append(bsoncxx::types::b_int64{course});
    }

    auto update_doc = make_document(
            kvp("$set",
                make_document(
                        kvp("token", user.user_token),
                        kvp("name", user.name),
                        kvp("discord_id", bsoncxx::types::b_int64{user.discord_id}),
                        kvp("courses", courses_array_builder.view())
                )
            )
    );

    auto update = DatabaseManager::user_collection.update_one(
            make_document(kvp("_id", bsoncxx::types::b_int64{user.user_id})),
            update_doc.view());
    
    assert(update);
    assert(update->modified_count() <= 1);
}

void DatabaseManager::insert_user(const User &user) {
    bsoncxx::builder::basic::array courses_array_builder;
    for (const long &course : user.courses) {
        courses_array_builder.append(bsoncxx::types::b_int64{course});
    }

    auto create_doc = make_document(
        kvp("_id", bsoncxx::types::b_int64{user.user_id}),
        kvp("token", user.user_token),
        kvp("name", user.name),
        kvp("discord_id", bsoncxx::types::b_int64{user.discord_id}),
        kvp("courses", courses_array_builder.view())
    );

    auto create = DatabaseManager::user_collection.insert_one(create_doc.view());

    assert(create);
}

void DatabaseManager::update_course(const Course &course) {
    bsoncxx::builder::basic::array guilds_array_builder;
    for (const long &guild : course.tracking_guilds) {
        guilds_array_builder.append(bsoncxx::types::b_int64{guild});
    }

    auto update_doc = make_document(
            kvp("$set",
                make_document(
                        kvp("_id", static_cast<int64_t>(course.course_id)),
                        kvp("name", course.name),
                        kvp("tracking_guilds", guilds_array_builder.view())
                )
            )
    );

    auto update = DatabaseManager::course_collection.update_one(
            make_document(kvp("_id", bsoncxx::types::b_int64{course.course_id})),
            update_doc.view());

    assert(update);
    assert(update->modified_count() <= 1);
}

void DatabaseManager::insert_course(const Course &course) {
    bsoncxx::builder::basic::array guilds_array_builder;
    for (const long &guild : course.tracking_guilds) {
        guilds_array_builder.append(bsoncxx::types::b_int64{guild});
    }

    auto create_doc = make_document(
            kvp("_id", bsoncxx::types::b_int64{course.course_id}),
            kvp("name", course.name),
            kvp("tracking_guilds", guilds_array_builder.view())
    );

    auto create = DatabaseManager::course_collection.insert_one(create_doc.view());

    assert(create);
}

void DatabaseManager::update_guild(const Guild &guild) {
    bsoncxx::builder::basic::array users_array_builder;
    for(const long &user : guild.verified_users) {
        users_array_builder.append(bsoncxx::types::b_int64{user});
    }

    bsoncxx::builder::basic::array courses_array_builder;
    for(const std::shared_ptr<TrackedCourse> &tracked_course : guild.tracked_courses) {

        bsoncxx::builder::basic::document course_doc_builder;
        course_doc_builder.append(kvp("course_id", static_cast<int64_t>(tracked_course->course_id)));
        course_doc_builder.append(kvp("category_id", static_cast<int64_t>(tracked_course->category_id)));
        course_doc_builder.append(kvp("announcements_channel", static_cast<int64_t>(tracked_course->announcements_channel)));
        course_doc_builder.append(kvp("forums_channel", static_cast<int64_t>(tracked_course->forums_channel)));
        course_doc_builder.append(kvp("role_id", static_cast<int64_t>(tracked_course->role_id)));
        courses_array_builder.append(course_doc_builder.extract());
    }

    auto update_doc = make_document(
            kvp("$set",
                make_document(
                        kvp("_id", static_cast<int64_t>(guild.guild_id)),
                        kvp("verified_role_id", static_cast<int64_t>(guild.verified_role_id)),
                        kvp("verified_users", users_array_builder.view()),
                        kvp("tracked_courses", courses_array_builder.view())
                )
            )
    );

    auto update = DatabaseManager::guild_collection.update_one(
            make_document(kvp("_id", bsoncxx::types::b_int64{guild.guild_id})),
            update_doc.view());

    assert(update);
    assert(update->modified_count() <= 1);
}

void DatabaseManager::insert_guild(const Guild &guild) {
    bsoncxx::builder::basic::array users_array_builder;
    for (const long &user : guild.verified_users) {
        users_array_builder.append(bsoncxx::types::b_int64{user});
    }

    bsoncxx::builder::basic::array courses_array_builder;
    for (const std::shared_ptr<TrackedCourse> &tracked_course : guild.tracked_courses) {
        bsoncxx::builder::basic::document course_doc_builder;
        course_doc_builder.append(kvp("course_id", static_cast<int64_t>(tracked_course->course_id)));
        course_doc_builder.append(kvp("category_id", static_cast<int64_t>(tracked_course->category_id)));
        course_doc_builder.append(kvp("announcements_channel", static_cast<int64_t>(tracked_course->announcements_channel)));
        course_doc_builder.append(kvp("forums_channel", static_cast<int64_t>(tracked_course->forums_channel)));
        course_doc_builder.append(kvp("role_id", static_cast<int64_t>(tracked_course->role_id)));
        courses_array_builder.append(course_doc_builder.extract());
    }

    auto create_doc =  make_document(
            kvp("_id", static_cast<int64_t>(guild.guild_id)),
            kvp("verified_role_id", static_cast<int64_t>(guild.verified_role_id)),
            kvp("verified_users", users_array_builder.view()),
            kvp("tracked_courses", courses_array_builder.view())
    );

    auto create = DatabaseManager::guild_collection.insert_one(create_doc.view());

    assert(create);
}

mongocxx::client DatabaseManager::client;
mongocxx::database DatabaseManager::database;
mongocxx::collection DatabaseManager::user_collection;
mongocxx::collection DatabaseManager::course_collection;
mongocxx::collection DatabaseManager::guild_collection;




