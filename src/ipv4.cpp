#include "ipv4.h"

#include <stdio.h>
#include <stdint.h>

enum IPProtocolType {
    ICMP = 1,
    TCP  = 6,
    UDP  = 17,
    STP  = 118
};

struct IPv4Package {
    uint8_t  ip_hl:4, ip_v:4;
    uint8_t  ip_tos;
    uint16_t ip_len;
    uint16_t ip_id;
    uint16_t ip_off;
    uint8_t  ip_ttl;
    uint8_t  ip_p;
    uint16_t ip_sum;
    uint32_t ip_src;
    uint32_t ip_dst;
    void* data;
};

void processIPv4(void* buffer, size_t size) {
    IPv4Package* package = (IPv4Package*)buffer;

    printf("src: %x\n", package->ip_src);
    printf("dst: %x\n", package->ip_dst);
    printf("protocol: ", package->ip_dst);
    
    switch (package->ip_p)
    {
        case ICMP:
            printf("ICMP\n");
            break;
        case TCP:
            printf("TCP\n");
            break;
        case UDP:
            printf("UDP\n");
            break;
        case STP:
            printf("STP\n");
            break;
    
        default:
            printf("Something other: %d\n", package->ip_p);
            break;
    }
}