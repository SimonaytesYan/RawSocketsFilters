#include "ipv4.h"

#include <stdio.h>
#include <stdint.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>

enum IPProtocolType {
    ICMP = 1,
    TCP  = 6,
    UDP  = 17,
    STP  = 118
};

void processUDP(void* buffer) {
    udphdr* package = (udphdr*)buffer;

    printf("src port: %d\n", package->source);
    printf("dst port: %d\n", package->dest);
}

void processTCP(void* buffer) {
    tcphdr* package = (tcphdr*)buffer;

    printf("src port: %d\n", package->source);
    printf("dst port: %d\n", package->dest);
}

void processIPv4(void* buffer) {
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
            processTCP((char*)buffer + sizeof(iphdr));
            break;
        case UDP:
            printf("UDP\n");
            processUDP((char*)buffer + sizeof(iphdr));
            break;
        case STP:
            printf("STP\n");
            break;
    
        default:
            printf("Something other: %d\n", package->protocol);
            break;
    }
}