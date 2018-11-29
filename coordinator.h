//
// Created by Linghan Xing on 11/28/18.
//

#ifndef KV_COORDINATOR_COORDINATOR_H
#define KV_COORDINATOR_COORDINATOR_H

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include "message.h"
#include "tcp_helper.h"

struct kv {
    std::string key;
    std::string value;
};
struct node_info {
    std::string addr;
    std::string port;
    bool alive;
    std::vector<struct kv> q;
};
class Coordinator {
protected:
    std::vector<node_info> ring;
    void add_node(std::string address, std::string port);
    void remove_node(std::string address, std::string port);
    void crash_node(std::string address, std::string port);
    std::tuple<struct node_info, struct node_info> find_node(std::string);
public:
    void init(std::string address, std::string port);
    std::string get(std::string key);
    void put(std::string key, std::string value);
    void join(std::string addr, std::string port);
    void kill(std::string addr, std::string port);
    void crash(std::string addr, std::string port);
};


#endif //KV_COORDINATOR_COORDINATOR_H
