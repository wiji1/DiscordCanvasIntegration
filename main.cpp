#include <iostream>
#include <string>
#include <dpp/dpp.h>
#include <mongocxx/instance.hpp>
#include "include/ImageFromHTML.h"
#include "include/ConfigManager.h"
#include "include/DatabaseManager.h"
#include "include/User.h"
#include "include/exceptions/DocumentNotFoundException.h"
#include "include/Guild.h"

std::string get_token();

std::unique_ptr<dpp::cluster> bot {nullptr};

int main() {
    ConfigManager::init();

    dpp::cluster bot_cluster(get_token());
    bot.reset(&bot_cluster);

    DatabaseManager::init();

    bot->on_log(dpp::utility::cout_logger());

    bot->on_ready([&](const dpp::ready_t & event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot->global_command_create(dpp::slashcommand("verify", "verification", bot->me.id));
            bot->global_command_create(dpp::slashcommand("test", "testing", bot->me.id));
            bot->global_command_create(dpp::slashcommand("setup", "testing", bot->me.id));
            bot->global_command_create(dpp::slashcommand("cleanup", "testing", bot->me.id));
            bot->global_command_create(dpp::slashcommand("save", "testing", bot->me.id));
            bot->global_command_create(dpp::slashcommand("ping", "testing", bot->me.id));
        }
    });

    bot->on_slashcommand([](const dpp::slashcommand_t &event)   {
        if(event.command.get_command_name() == "verify") {
            if(!Guild::is_registered(static_cast<long>(event.command.guild_id))) {
                event.reply("This guild is not registered!");
                return;
            }

            std::shared_ptr<User> user {nullptr};

            try {
                user = User::get_user(static_cast<long>(event.command.usr.id));
                event.thinking(true);
            } catch(const DocumentNotFoundException &ex) {
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
                return;
            }

            Guild &guild {*Guild::get_guild(static_cast<long>(event.command.guild_id))};

            if(std::find(guild.verified_users.begin(), guild.verified_users.end(), user->discord_id)!= guild.verified_users.end()) {
                event.edit_response("You are already verified!");
                return;
            }

            [](User u, long gid, dpp::slashcommand_t e) -> dpp::job {
                Guild &guild = {*Guild::get_guild(gid)};
                co_await guild.verify_user(u.discord_id, false);
                dpp::message message = {"Successfully Verified!"};
                co_await e.co_edit_response(message);
            }(*user, guild.guild_id, event);
        }

        if(event.command.get_command_name() == "test") {

//            setenv("LANG", "C", 1);


            std::cout << "Posting embed!" << std::endl;
            dpp::embed image = dpp::embed();
            image.set_image("attachment://output.jpg");
            image.set_title("title");
            image.set_url("https://google.com");
            image.set_color(dpp::colors::red);
            image.set_footer(dpp::embed_footer().set_text("author"));
            image.set_timestamp(time(nullptr));

            dpp::message msg(event.command.channel_id, image);
            msg.add_file("output.jpg", dpp::utility::read_file("output.jpg"));
            msg.set_channel_id(event.command.channel_id);
            bot->message_create(msg);

            User &user {*User::get_user(static_cast<long>(event.command.usr.id))};

            [](User *u) -> dpp::job {
                std::vector<dpp::task<void>> tasks;
                tasks.reserve(u->courses.size());

                for(const auto &item: u->courses) {
                    try {
                        Course &course {*Course::get_course(item)};

                        tasks.emplace_back(course.update());
                    } catch(DocumentNotFoundException &ex) { }
                }

                for(auto &task : tasks) co_await task;
            }(&user);



        }

        if(event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }

        if(event.command.get_command_name() == "setup") {
            long guild_id = event.command.guild_id;

            if(!event.command.get_guild().is_community()) {
                event.reply("Community must be enabled!");
                return;
            }

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

            Guild &guild = *Guild::get_guild(guild_id);
            guild.save();
        }
        if(event.command.get_command_name() == "cleanup") {
            long guild_id = event.command.guild_id;
            if(!Guild::is_registered(guild_id)) {
                event.reply("Guild is not registered!");
                return;
            }

            Guild &guild = *Guild::get_guild(guild_id);

            event.reply("Cleaning up!");
            guild.deregister();
        }
    });

    bot->on_form_submit([&](const dpp::form_submit_t & event) {
        std::string v = std::get<std::string>(event.components[0].components[0].value);
        dpp::message m;

        if(!Guild::is_registered(event.command.guild_id)) {
            m.set_content("This guild is not registered!").set_flags(dpp::m_ephemeral);
            event.edit_response(m);
            return;
        }

        m.set_content("You entered: " + v).set_flags(dpp::m_ephemeral);
        event.thinking(true);

        User &user {*User::create_user(v, static_cast<long>(event.command.get_issuing_user().id))};

        Guild &guild = *Guild::get_guild(event.command.guild_id);

//        [&user, &guild]() -> dpp::job {co_await guild.verify_user(user.discord_id, true);}();

        [](User u, long gid, dpp::form_submit_t e) -> dpp::job {
            Guild &guild = {*Guild::get_guild(gid)};
            co_await guild.verify_user(u.discord_id, true);
            dpp::message message = {"Successfully Verified!"};
            co_await e.co_edit_response(message);
        }(user, guild.guild_id, event);
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

void make_image(std::string &html) {
    wkhtmltoimage_global_settings * gs;
    wkhtmltoimage_converter * c;
    const unsigned char * data;
    long len;
}




