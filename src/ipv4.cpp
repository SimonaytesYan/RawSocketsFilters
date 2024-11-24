#include "ipv4.h"

#include <stdio.h>
#include <stdint.h>
#include <linux/ip.h>

enum IPProtocolType {
    ICMP = 1,
    TCP  = 6,
    UDP  = 17,
    STP  = 118
};

void processIPv4(void* buffer, size_t size) {
    iphdr* package = (iphdr*)buffer;

    printf("src: %x\n", package->saddr);
    printf("dst: %x\n", package->daddr);
    printf("protocol: ");
    
    switch (package->protocol)
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
            printf("Something other: %d\n", package->protocol);
            break;
    }
}