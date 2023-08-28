#ifndef LEARNINGCPP_USER_H
#define LEARNINGCPP_USER_H

#include <string>
#include <vector>

class User {
public:
    explicit User(std::string  user_token, long discord_id);
    explicit User(long discord_id);

    void update();
    void save() const;

    long discord_id;
    std::string user_token;
    long user_id{};
    std::string name;

    std::vector<long> courses;
};


#endif
