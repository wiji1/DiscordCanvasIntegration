#ifndef DISCORDCANVASINTEGRATION_USER_H
#define DISCORDCANVASINTEGRATION_USER_H

#include <string>
#include <vector>

class User {
public:
    long discord_id;
    std::string user_token;
    long user_id{};
    std::string name;

    std::vector<long> courses;

    explicit User(std::string  user_token, long discord_id);
    explicit User(long discord_id);

    void update();
    void save() const;
};


#endif
