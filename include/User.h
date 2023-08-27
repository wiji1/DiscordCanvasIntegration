#ifndef LEARNINGCPP_USER_H
#define LEARNINGCPP_USER_H

#include <string>
#include <vector>

class User {
public:
    explicit User(const std::string& user_token, long discord_id);

    long discord_id;
    long user_id;
    std::string name;

    std::vector<long> courses;
};


#endif
