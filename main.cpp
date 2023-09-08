#include <iostream>
#include <string>
#include <dpp/dpp.h>
#include <mongocxx/instance.hpp>
#include "include/ConfigManager.h"
#include "include/DatabaseManager.h"
#include "include/User.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include "include/Guild.h"

std::string get_token();

std::unique_ptr<dpp::cluster> bot {nullptr};

int main() {
    ConfigManager::init();
    DatabaseManager::init();

    dpp::cluster bot_cluster(get_token());
    bot.reset(&bot_cluster);


    bot->on_log(dpp::utility::cout_logger());

    bot->on_ready([&](const dpp::ready_t & event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot->global_command_create(dpp::slashcommand("verify", "verification", bot->me.id));
            bot->global_command_create(dpp::slashcommand("test", "testing", bot->me.id));
            bot->global_command_create(dpp::slashcommand("setup", "testing", bot->me.id));
            bot->global_command_create(dpp::slashcommand("cleanup", "testing", bot->me.id));
            bot->global_command_create(dpp::slashcommand("save", "testing", bot->me.id));
        }
    });

    bot->on_slashcommand([](const dpp::slashcommand_t & event) {
        if (event.command.get_command_name() == "verify") {
            dpp::interaction_modal_response modal("verification-form", "We need the following information from you");
            modal.add_component(
                    dpp::component().
                            set_label("Canvas Access Token").
                            set_id("access-token").
                            set_type(dpp::cot_text).
                            set_placeholder("Don't know how? Visit https://youtube.com/kys").
                            set_min_length(5).
                            set_max_length(100).
                            set_text_style(dpp::text_short)
            );
            modal.add_row();
            modal.add_component(
                    dpp::component().
                            set_label("Type rammel").
                            set_id("field_id2").
                            set_type(dpp::cot_text).
                            set_placeholder("gumf").
                            set_min_length(1).
                            set_max_length(2000).
                            set_text_style(dpp::text_paragraph)
            );
            event.dialog(modal);
        }

        if(event.command.get_command_name() == "test") {

            try {
                User user {User::get_user(static_cast<long>(event.command.get_issuing_user().id))};
//            User user {static_cast<long>(event.command.get_issuing_user().id)};
                event.reply(std::string("Hi"));
//            std::cout << user.discord_id << " " << user.user_id << " " << user.name << " [";
//            for (const auto &item: user.courses) std::cout << item << ", ";
//            std::cout << "] " << std::endl;


                user.update();
                user.save();
            } catch(DocumentNotFoundException &ex) { std::cout << ex.what() << std::endl; }
        }

        if(event.command.get_command_name() == "setup") {
            long guild_id = event.command.guild_id;
            if(Guild::is_registered(guild_id)) {
                event.reply("This guild is already registered!");
                return;
            }

            Guild::register_guild(guild_id);
            event.reply("Guild has been set up!");
        }
        if(event.command.get_command_name() == "save") {
            long guild_id = event.command.guild_id;
            if(!Guild::is_registered(guild_id)) {
                event.reply("Guild is not registered!");
                return;
            }

            Guild guild  = *Guild::get_guild(guild_id);
            guild.save();
        }
        if(event.command.get_command_name() == "cleanup") {
            long guild_id = event.command.guild_id;
            if(!Guild::is_registered(guild_id)) {
                event.reply("Guild is not registered!");
                return;
            }

            Guild guild = *Guild::get_guild(guild_id);
            std::cout << "Tracked Courses size: " << guild.tracked_courses.size() << std::endl;


            std::vector<std::shared_ptr<TrackedCourse>> tracked_courses {guild.tracked_courses};

            for (const auto &item: tracked_courses) {
                std::cout << item << " " << "Tracked Course ID: " << item->course_id << std::endl;
                guild.remove_tracked_course(item);
            }

            event.reply("Cleaning up!");
        }
    });

    bot->on_form_submit([&](const dpp::form_submit_t & event) {
        std::string v = std::get<std::string>(event.components[0].components[0].value);
        dpp::message m;

        if(!Guild::is_registered(event.command.guild_id)) {
            m.set_content("This guild is not registered!").set_flags(dpp::m_ephemeral);
            event.reply(m);
            return;
        }

        m.set_content("You entered: " + v).set_flags(dpp::m_ephemeral);
        event.reply(m);

        User user {User::create_user(v, static_cast<long>(event.command.get_issuing_user().id))};


        user.save();
        Guild &guild = *Guild::get_guild(event.command.guild_id);
        guild.verified_users.push_back(user.discord_id);
        guild.update();
    });

    bot->start(dpp::st_wait);

    return 0;
}

std::string get_token() {
    ConfigManager::init();
    if (!ConfigManager::data.contains("bot-token")) throw std::runtime_error {"Config file does not contain bot token"};

    std::string token {ConfigManager::data["bot-token"]};
    return token;
}




