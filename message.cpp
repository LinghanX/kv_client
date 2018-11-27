//
// Created by Linghan Xing on 11/26/18.
//

#include "message.h"

msg_type check_msg_type(void *msg, uint32_t size) {
    const auto logger = spdlog::get("console");
    auto *first_int = (uint32_t *) msg;
    *first_int = ntohl(*first_int);

    if (size == sizeof(get_msg) && *first_int == 0) {
        return msg_type::GET;
    } else if (size == sizeof(put_msg) && *first_int == 1){
        return msg_type::PUT;
    } else if (size == sizeof(kill_msg) && *first_int == 2) {
        return msg_type::KILL;
    } else if (size == sizeof(join_msg) && *first_int == 3) {
        return msg_type::JOIN;
    } else {
        logger -> error("unable to identify incoming message");
        return msg_type::ERROR;
    }
}
