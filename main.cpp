#include <iostream>
#include "include/spdlog/spdlog.h"
#include "include/spdlog/sinks/stdout_color_sinks.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "message.h"

#define MAXDATASIZE 1024

void print_all(std::vector<std::string> tokens) {
    for (const auto& n : tokens) {
        std::cout << n << std::endl;
    }
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::string make_tcp_query(
        msg_type type,
        std::string address,
        std::string port_num,
        std::string key,
        std::string value) {

    const auto console = spdlog::get("console");
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(address.c_str(), port_num.c_str(), &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo);

    std::cout << "start session" << std::endl;

    switch (type) {
        case msg_type::GET: {
            get_msg msg;
            msg.type = 0;
            std::size_t len = key.copy(msg.key, key.length(), 0);
            msg.key[len] = '\0';
            console -> info("sending msg");
            if (send(sockfd, &msg, sizeof(get_msg), 0) <= 0)
                console -> error("unable to send");
            console -> info("i am here");

            if (recv(sockfd, buf, sizeof(buf), 0) <= 0)
                console -> error("no response");

            std::string res(buf);
            std::cout << "response is: " << buf << std::endl;
            return res;
        }
        case msg_type::PUT: {
            put_msg msg;
            msg.type = 1;
            htonl(msg.type);
            strcpy(msg.key, key.c_str());
            strcpy(msg.value, value.c_str());

            console -> info("sending msg: type: {}, key: {}, val: {}, size: {}",
                            msg.type, msg.key, msg.value, sizeof (put_msg));
            if (send(sockfd, &msg, sizeof(put_msg), 0) <= 0)
                console -> error("unable to send");

            return "successful";
        }
        default:
            return "error";
    }
}

int main(int argc, char** argv) {
    auto console = spdlog::stdout_color_mt("console");
    console -> info("welcome to my key value store!");

    // --------- start connecting ------
    std::string address, port_num, user_input;
    std::tie(address, port_num) = handle_input(argc, argv);

    while (true) {
        getline(std::cin, user_input);

        std::vector<std::string> tokens;

        char *input = strdup(user_input.c_str());
        char *token = strtok(input, " ");

        while (token != nullptr) {
            tokens.push_back(token);
            token = strtok(NULL, " ");
        }

        std::string command = tokens[0];

        print_all(tokens);

        if (command == "get") {
            std::cout << make_tcp_query(msg_type::GET, address, port_num, tokens[1], "null") << std::endl;
        } else if (command == "put") {
            std::cout << make_tcp_query(msg_type::PUT, address, port_num, tokens[1], tokens[2]) << std::endl;
        } else if (command == "join") {
            print_all(tokens);
        } else if (command == "kill") {
            print_all(tokens);
        } else {
            console -> error("unable to recognize command");
        }

        std::memset(&user_input, 0, sizeof user_input);
    }
}