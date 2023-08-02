#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stack>
#include <dpp/dpp.h>
#include <exception>

std::string get_token();

const std::string BOT_TOKEN {get_token()};

int main() {
    dpp::cluster bot(BOT_TOKEN);
    bot.on_log(dpp::utility::cout_logger());

    bot.on_slashcommand([](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "ping") {
            event.reply("Pong!");
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_bulk_command_create(std::vector<dpp::slashcommand> {
                    dpp::slashcommand("ping", "Ping pong!", bot.me.id),
                    dpp::slashcommand("test", "A development command", bot.me.id)
            }
            );
        }
    });

    bot.start(dpp::st_wait);


    return 0;
}

std::string get_token() {
    std::ifstream input {std::ifstream {"../token.txt"}};
    if(!input) throw std::exception {};

    std::string token;
    std::getline(input, token);
    return token;
}



