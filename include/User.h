    #ifndef DISCORDCANVASINTEGRATION_USER_H
    #define DISCORDCANVASINTEGRATION_USER_H

    #include <string>
    #include <vector>
    #include <unordered_map>

    class User {
    private:
        static std::unordered_map<long, std::unique_ptr<User>> user_map;
    public:
        long discord_id;
        std::string user_token;
        long user_id{};
        std::string name;

        std::vector<long> courses;

        static User &create_user(std::string user_token, long discord_id);
        static User &get_user(long discord_id);

        void update();
        void save() const;

        explicit User(std::string user_token, long discord_id);
        explicit User(long discord_id);
    };


    #endif
