#include <iostream>
#include "include/spdlog/spdlog.h"
#include "include/spdlog/sinks/stdout_color_sinks.h"
#include "message.h"
#include "tcp_helper.h"
#include "coordinator.h"

#define MAXDATASIZE 1024

std::vector<std::string> parse_user_input(std::string user_input) {
    std::vector<std::string> tokens;
    char *input = strdup(user_input.c_str());
    char *token = strtok(input, " ");

    while (token != nullptr) {
        tokens.push_back(token);
        token = strtok(NULL, " ");
    }

    return tokens;
}

int main(int argc, char** argv) {
    auto console = spdlog::stdout_color_mt("console");
    console -> info("welcome to my key value store!");

    // --------- start connecting ------
    std::string address, port_num, user_input;
    std::tie(address, port_num) = handle_input(argc, argv);
    Coordinator coordinator;
    coordinator.init(address, port_num);

    while (true) {
        getline(std::cin, user_input);

        std::vector<std::string> tokens = parse_user_input(user_input);
        std::string command = tokens[0];

        if (command == "get") {
            auto val = coordinator.get(tokens[1]);
            console -> info("value is: {}", val);
        } else if (command == "put") {
            coordinator.put(tokens[1], tokens[2]);
        } else if (command == "join") {
            coordinator.join(tokens[1], tokens[2]);
        } else if (command == "kill") {
            coordinator.kill(tokens[1], tokens[2]);
        } else if (command == "crash") {
            coordinator.crash(tokens[1], tokens[2]);
        } else if (command == "reboot") {
            coordinator.reboot(tokens[1], tokens[2]);
        } else {
            console -> error("unable to recognize command");
        }

        std::memset(&user_input, 0, sizeof user_input);
    }
}