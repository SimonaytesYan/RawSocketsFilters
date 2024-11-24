#pragma once

#include <stdint.h>
#include <vector>

#include "ipv4.h"

enum class FilterRuleProtocol {
    UDP,
    TCP,
    NOT_STATED
};

enum class RuleType {
    PASS,
    DELETE
};

const uint8_t kNotStated = -1;

struct Package {
    uint32_t src_ip;
    uint32_t dst_ip;

    uint8_t src_port;
    uint8_t dst_port;

    IPProtocolType protocol;
};

struct FilterRule {
    Package mask;
    RuleType type;
};

void printPackage(Package pkg);

void filter(int in_socket, int out_socket, FilterRule rule);
