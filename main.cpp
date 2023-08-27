#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <stack>
#include <dpp/dpp.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "include/ConfigManager.h"
#include "include/DatabaseManager.h"
#include "include/User.h"

std::string get_token();
void make_request(const std::string &token);

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
    });

    bot->on_form_submit([&](const dpp::form_submit_t & event) {
        /* For this simple example we know the first element of the first row ([0][0]) is value type string.
         * In the real world it may not be safe to make such assumptions!
         */
        std::string v = std::get<std::string>(event.components[0].components[0].value);
        dpp::message m;

        User user {v, static_cast<long>(event.command.get_issuing_user().id)};
        std::cout << user.discord_id << " " << user.user_id << " " << user.name << " [";
        for (const auto &item: user.courses) std::cout << item << ", ";
        std::cout << "] " << std::endl;


        user.save();

        m.set_content("You entered: " + v).set_flags(dpp::m_ephemeral);
        event.reply(m);
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

void make_request(const std::string &token) {
    bot->request(
            "https://canvas.instructure.com/api/v1/users/self/profile", dpp::m_get, [](const dpp::http_request_completion_t & cc) {
//            "https://canvas.instructure.com/api/v1/courses", dpp::m_get, [](const dpp::http_request_completion_t & cc) {
                // This callback is called when the HTTP request completes. See documentation of
                // dpp::http_request_completion_t for information on the fields in the parameter.
                std::cout << "I got reply: " << cc.body << " with HTTP status code: " << cc.status << "\n";
            }, "",
            "application/json",
            {
                    {"Authorization", "Bearer " + token }
            }
    );
}




