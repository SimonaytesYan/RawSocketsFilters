#pragma once

enum class IPProtocolType {
    ICMP = 1,
    TCP  = 6,
    UDP  = 17,
    STP  = 118,

    BROKEN = 255,
};

struct FilterList;

// return true, if package should go
//        false if package shouldn't go
bool processIPv4(void* buffer, const FilterList& rules);
