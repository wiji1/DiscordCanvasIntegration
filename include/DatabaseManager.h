#ifndef LEARNINGCPP_DATABASEMANAGER_H
#define LEARNINGCPP_DATABASEMANAGER_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

class DatabaseManager {
public:
    static mongocxx::client client;
    static mongocxx::database database;

    static mongocxx::collection user_collection;
    static mongocxx::collection course_collection;

    static void init();

};


#endif
