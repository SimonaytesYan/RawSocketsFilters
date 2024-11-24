#include "ipv4.h"
#include "filter.h"

#include <stdio.h>
#include <stdint.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>


bool processUDP(void* buffer, FilterRule rule) {
    udphdr* package = (udphdr*)buffer;

    printf("src port: %d\n", package->source);
    printf("dst port: %d\n", package->dest);

    if (rule.dst_port != kNotStated && rule.dst_port != package->source)
        return false;
    if (rule.src_port != kNotStated && rule.src_port != package->dest)
        return false;

    return true;
}

bool processTCP(void* buffer, FilterRule rule) {
    tcphdr* package = (tcphdr*)buffer;

    printf("src port: %d\n", package->source);
    printf("dst port: %d\n", package->dest);

    if (rule.dst_port != kNotStated && rule.dst_port != package->source)
        return false;
    if (rule.src_port != kNotStated && rule.src_port != package->dest)
        return false;

    return true;
}

bool processIPv4(void* buffer, FilterRule rule) {
    iphdr* package = (iphdr*)buffer;
    IPProtocolType protocol = (IPProtocolType)package->protocol;

    printf("src: %x\n", package->saddr);
    printf("dst: %x\n", package->daddr);
    printf("protocol: ");

    if (rule.dst_ip != kNotStated && rule.dst_ip != package->daddr)
        return false;
    if (rule.src_ip != kNotStated && rule.src_ip != package->saddr)
        return false;
    
    if (rule.protocol == FilterRuleProtocol::TCP && protocol != IPProtocolType::TCP)
        return false;
    if (rule.protocol == FilterRuleProtocol::UDP && protocol != IPProtocolType::UDP)
        return false;
    
    switch (protocol)
    {
        case IPProtocolType::ICMP:
            printf("ICMP\n");
            break;
        case IPProtocolType::TCP:
            printf("TCP\n");
            return processTCP((char*)buffer + sizeof(iphdr), rule);
            break;
        case IPProtocolType::UDP:
            printf("UDP\n");
            return processUDP((char*)buffer + sizeof(iphdr), rule);
            break;
        case IPProtocolType::STP:
            printf("STP\n");
            break;
    
        default:
            printf("Something other: %d\n", package->protocol);
            break;
    }

    return true;
}