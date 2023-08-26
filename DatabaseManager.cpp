#include "DatabaseManager.h"
#include "ConfigManager.h"

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

    auto insert_one_result = user_collection.insert_one(make_document(kvp("i", 0)));
    assert(insert_one_result);
}

mongocxx::client DatabaseManager::client;
mongocxx::database DatabaseManager::database;
mongocxx::collection DatabaseManager::user_collection;
mongocxx::collection DatabaseManager::course_collection;




