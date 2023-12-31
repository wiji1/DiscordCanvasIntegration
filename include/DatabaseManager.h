#ifndef DISCORDCANVASINTEGRATION_DATABASEMANAGER_H
#define DISCORDCANVASINTEGRATION_DATABASEMANAGER_H

#include "User.h"
#include "Course.h"
#include "Guild.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <string>

class DatabaseManager {
public:
    static mongocxx::client client;
    static mongocxx::database database;

    static mongocxx::collection user_collection;
    static mongocxx::collection course_collection;
    static mongocxx::collection guild_collection;

    static void init();
    static void load_guilds();

    static bsoncxx::document::value fetch_user_document(long id);
    static bsoncxx::document::value fetch_course_document(long id);
    static bsoncxx::document::value fetch_guild_document(long id);

    static void update_user(const User &user);
    static void insert_user(const User &user);

    static void update_course(const Course &course);
    static void insert_course(const Course &course);
    static void delete_course(const Course &course);

    static void update_guild(const Guild &guild);
    static void insert_guild(const Guild &guild);
    static void delete_guild(const Guild &guild);

};


#endif
