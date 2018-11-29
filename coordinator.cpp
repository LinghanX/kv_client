//
// Created by Linghan Xing on 11/28/18.
//

#include "coordinator.h"
#define TABLESIZE 1048576

size_t hash_node(struct node_info node) {
    std::hash<std::string> hash_str;
    return hash_str(node.addr + node.port) % TABLESIZE;
}
void Coordinator::add_node(std::string address, std::string port) {
    struct node_info local_node;
    local_node.port = std::move(port);
    local_node.addr = std::move(address);
    local_node.alive = true;

    this -> ring.push_back(local_node);
    std::sort(this -> ring.begin(), this -> ring.end(),
            [](node_info a, node_info b) {
        return hash_node(a) < hash_node(b); });
}
void Coordinator::remove_node(std::string address, std::string port) {
    this -> ring.erase(
            std::remove_if(
                    this -> ring.begin(),
                    this -> ring.end(),
                    [&](node_info node){
                        return node.addr == address && node.port == port;
                    }),
            this -> ring.end());
}
void Coordinator::crash_node(std::string address, std::string port) {
    for (auto& node : this -> ring) {
        if (node.addr == address && node.port == port) {
            node.alive = false;
        }
    }
}
void Coordinator::reboot(std::string addr, std::string port) {
    for (auto& node : this -> ring) {
        if (node.addr == addr && node.port == port) {
            node.alive = true;
            for (auto entry : node.q) {
                make_tcp_query(msg_type::PUT, node.addr, node.port, entry.key, entry.value);
            }
        }
    }
}
void Coordinator::init(std::string address, std::string port) {
    this -> add_node(std::move(address), std::move(port));
}
void Coordinator::join(std::string addr, std::string port) {
    this -> add_node(std::move(addr), std::move(port));
}
void Coordinator::kill(std::string addr, std::string port) {
    this -> remove_node(addr, port);
}
void Coordinator::crash(std::string addr, std::string port) {
    this -> crash_node(addr, port);
}
std::tuple<struct node_info, struct node_info> Coordinator::find_node(std::string key) {
    auto console = spdlog::get("console");
    std::hash<std::string> hash_str;
    size_t size = this -> ring.size();
    size_t key_hash = hash_str(key) % TABLESIZE;

    console -> info("we have a ring");
    for (const auto &n : this -> ring) {
        console -> info("addr: {}, port: {}, hash: {}",
                n.addr, n.port, hash_node(n));
    }

    struct node_info last_node = this -> ring[size - 1];
    struct node_info curr_node = this -> ring[0];
    if (size == 0) perror("no valid node in the ring");

    if ((key_hash < TABLESIZE && key_hash >= hash_node(last_node))
        || (key_hash >= 0 && key_hash < hash_node(curr_node))) {
        // last_node is the target node
        return std::tie(this -> ring[size - 1], this -> ring[0]);
    }

    for (int i = 1; i < size; i++) {
        last_node = this -> ring[i - 1];
        curr_node = this -> ring[i];

        if (key_hash >= hash_node(last_node) && key_hash < hash_node(curr_node)) {
            return std::tie(last_node, curr_node);
        }
    }
    perror("unable to find node");
}
std::string Coordinator::get(std::string key) {
    struct node_info pre, curr;
    std::tie(pre, curr) = find_node(key);

    if (pre.alive)
        return make_tcp_query(msg_type::GET, pre.addr, pre.port, key, "null");
    else {
        return make_tcp_query(msg_type::GET, curr.addr, curr.port, key, "null");
    }
}
void Coordinator::push_entry(struct node_info node, struct kv entry) {
    for (auto& curr : this -> ring) {
        if (curr.addr == node.addr && curr.port == node.port) {
            curr.q.push_back(entry);
        }
    }
}
void Coordinator::put(std::string key, std::string value) {
    struct node_info pre, curr;
    std::tie(pre, curr) = find_node(key);
    if (pre.alive)
        make_tcp_query(msg_type::PUT, pre.addr, pre.port, key, value);
    else {
        struct kv entry;
        entry.key = key;
        entry.value = value;
        push_entry(pre, entry);
    }
    if (curr.alive)
        make_tcp_query(msg_type::PUT, curr.addr, curr.port, key, value);
    else {
        struct kv entry;
        entry.key = key;
        entry.value = value;
        push_entry(curr, entry);
    }
}
