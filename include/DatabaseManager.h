#ifndef LEARNINGCPP_DATABASEMANAGER_H
#define LEARNINGCPP_DATABASEMANAGER_H

#include "User.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <string>

class DatabaseManager {
public:
    static mongocxx::client client;
    static mongocxx::database database;

    static mongocxx::collection user_collection;
    static mongocxx::collection course_collection;

    static void init();
    static bsoncxx::document::view fetch_user_document(long id);
    static std::string fetch_course_document(long id);

    static void update_user(const User& user);
    static void insert_user(const User& user);

};


#endif
