//
// Created by Linghan Xing on 11/26/18.
//

#ifndef KV_CLIENT_MESSAGE_H
#define KV_CLIENT_MESSAGE_H

#include <cstdint>
#include <spdlog/spdlog.h>

enum msg_type {
    GET,
    PUT,
    KILL,
    JOIN,
    CRASH,
    REBOOT,
    ERROR
};

typedef struct {
    uint32_t type; // 0
    char key[256];
} get_msg;

typedef struct {
    uint32_t type; // 1
    char key[256];
    char value[256];
} put_msg;

typedef struct {
    uint32_t type; // 2
    char addr[256];
    char port[256];
} kill_msg;

typedef struct {
    uint32_t type; // 3
    char addr[256];
    char port[256];
} join_msg;

msg_type check_msg_type(void *msg, uint32_t size);

#endif //KV_CLIENT_MESSAGE_H
