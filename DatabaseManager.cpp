#include "include/DatabaseManager.h"
#include "include/ConfigManager.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include <optional>

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
                        kvp("id_", static_cast<int64_t>(course.course_id)),
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
            kvp("courses", guilds_array_builder.view())
    );

    auto create = DatabaseManager::course_collection.insert_one(create_doc.view());

    assert(create);
}


mongocxx::client DatabaseManager::client;
mongocxx::database DatabaseManager::database;
mongocxx::collection DatabaseManager::user_collection;
mongocxx::collection DatabaseManager::course_collection;




