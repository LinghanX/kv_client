//
// Created by Linghan Xing on 11/25/18.
//

#include "parse.h"

std::tuple<std::string, std::string> handle_input(int argc, char **argv)
{
    auto logger = spdlog::get("console");

    int option;
    int pflag = 0;
    int aflag = 0;
    std::string portNum;
    std::string address;

    while ((option = getopt(argc, argv, "p:a:")) != -1) {
        switch (option) {
            case 'p':
                portNum = optarg;
                pflag = 1;
                break;
            case 'a':
                address = optarg;
                aflag = 1;
                break;
            default:
                logger -> error("error");
        }
    }


    if (aflag == 0 || pflag == 0) {
        logger -> error("please input all required options");
        exit(1);
    }

    if (stoi(portNum) < 1024 ||  stoi(portNum) > 65535) {
        logger -> error("PortNumber out of range", portNum);
        exit(1);
    }

    logger -> info("finished processing input");
    logger -> info("the selected portal is: {}", portNum);
    logger -> info("the selected address is: {}", address);

    return std::make_tuple(address, portNum);
}
