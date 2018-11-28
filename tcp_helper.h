//
// Created by Linghan Xing on 11/28/18.
//

#ifndef KV_COORDINATOR_TCP_HELPER_H
#define KV_COORDINATOR_TCP_HELPER_H


#include <string>
#include "message.h"
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

std::string make_tcp_query( msg_type type, std::string address, std::string port_num, std::string key, std::string value);

#endif //KV_COORDINATOR_TCP_HELPER_H
