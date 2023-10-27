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
//            ImageFromHTML::init();
//
//
//            dpp::embed image = dpp::embed();
//            image.set_image("attachment://output.jpg");
//            image.set_title("CELLOBRATION 2023 is HERE at ASA on Saturday, November 4th - COME JOIN THE FUN!");
//            image.set_url("https://goasa.instructure.com/courses/5218/discussion_topics/33521");
//            image.set_color(dpp::colors::red);
//            image.set_footer(dpp::embed_footer().set_text("Message"));
//            image.set_timestamp(time(nullptr));
//
//            dpp::message msg(event.command.channel_id, "Hey there, I've got a new file!");
//            msg.add_file("output.jpg", dpp::utility::read_file("output.jpg"));
//
//            dpp::channel channel = event.command.channel;
//            bot->message_create(dpp::message(event.command.channel_id, image).set_reference(event.command.id).set_channel_id(channel.id).add_file("output.jpg", dpp::utility::read_file("output.jpg")));
//
//            return;
//
////            dpp::message(event.command.channel_id, embed).set_reference(event.command.id).set_channel_id(channel.id)
//
//            dpp::embed embed = dpp::embed().
//                    set_color(dpp::colors::red).
//                    set_title("CELLOBRATION 2023 is HERE at ASA on Saturday, November 4th - COME JOIN THE FUN!").
//                    set_url("https://goasa.instructure.com/courses/5218/discussion_topics/33521").
//                    add_field(
//                    "Content:",
//                    "Hello AZ Cellists!\n"
//                    "The Cellobration 2023 team hit the road and found our new home at Arizona School for the Arts. We can't wait to see you again and celebrate OUR CELLO COMMUNITY!\n"
//                    "Register by Saturday, October 7th to ensure that you receive the right T-shirt size.\n"
//                    "Here are the details:\n"
//                    "When is it you ask? Saturday, November 4, 2023\n"
//                    "Where is it located? Arizona School for the Arts Links to an external site.\n"
//                    "Will EVERYONE receive a T-shirt? YES - register by 10/7/23 to ensure that you get the correct size.\n"
//                    "Will there be an Audition Prep Workshop? YES (this year, like last year, it is after the conclusion of our 1:30pm concert)\n"
//                    "We have added a \"Shooting Stars\" session for our young cellists, ages 4-7.\n"
//                    "Will we wear masks indoors? Masks are optional, HOWEVER, if the metrics for the community warrant the wearing of masks, YES, yes we will! This will be reviewed as we get closer to the date.\n"
//                    "What about the ACS Cello Competition? YES! It will take place on Friday, November 3rd\n"
//                    "Sign up by October 7th to secure your T-shirt size!\n"
//                    "How do you find out more? Visit:\n"
//                    "http://www.cellobrationaz.com Links to an external site.  AND/OR \n"
//                    "Cellobration 2023 Information Links to an external site. and\n"
//                    "Arizona Cello Society Competition 2023 Links to an external site.\n"
//                    "We can't wait to see and hear you. Register Links to an external site. today!"
//            ).
//                    set_footer(dpp::embed_footer().set_text("Message")).
//                    set_timestamp(time(nullptr));
//
////            dpp::channel channel = event.command.channel;
//            bot->message_create(dpp::message(event.command.channel_id, embed).set_reference(event.command.id).set_channel_id(channel.id));
//
//            event.reply(dpp::message(event.command.channel_id, embed).set_reference(event.command.id).set_channel_id(channel.id));

            User &user {*User::get_user(static_cast<long>(event.command.usr.id))};
            for(const auto &item: user.courses) {
                try {
                    Course &course {*Course::get_course(item)};

                    [](Course *c) -> dpp::job {
                        co_await c->update();
                    }(&course);

                } catch(DocumentNotFoundException &ex) { }
            }

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




