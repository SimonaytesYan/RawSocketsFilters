#pragma once

#include "filter.h"

enum class IPProtocolType {
    ICMP = 1,
    TCP  = 6,
    UDP  = 17,
    STP  = 118
};

// return true, if package follow rule
//        false otherwise
bool processIPv4(void* buffer, FilterRule rule);
