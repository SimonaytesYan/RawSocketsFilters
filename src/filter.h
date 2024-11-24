#pragma once

#include <stdint.h>

enum class FilterRuleProtocol {
    UDP,
    TCP,
    NOT_STATED
};

const uint8_t kNotStated = -1;

struct FilterRule {
    uint32_t src_ip;
    uint32_t dst_ip;

    uint8_t src_port;
    uint8_t dst_port;

    FilterRuleProtocol protocol;
};

void filter(int in_socket, int out_socket, FilterRule rule);
