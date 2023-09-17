#ifndef DISCORDCANVASINTEGRATION_USER_H
#define DISCORDCANVASINTEGRATION_USER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <dpp.h>

class User {
private:
    static std::unordered_map<long, std::shared_ptr<User>> user_map;
public:
    long discord_id;
    std::string user_token;
    long user_id;
    std::string name;

    std::vector<long> courses;

    static std::shared_ptr<User> &create_user(std::string user_token, long discord_id);
    static std::shared_ptr<User> &get_user(long discord_id);

    dpp::task<void> update();
    void save() const;

    explicit User(std::string user_token, long discord_id);
    explicit User(long discord_id);
};


#endif
