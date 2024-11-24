#include "ipv4.h"
#include "filter.h"

#include <stdio.h>
#include <stdint.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <netinet/in.h>

void processUDP(void* buffer, Package& res_package) {
    udphdr* package = (udphdr*)buffer;

    res_package.src_port = ntohs(package->source);
    res_package.dst_port = ntohs(package->dest);

    printf("src port: %u\n", res_package.src_port);
    printf("dst port: %u\n", res_package.dst_port);
}

void processTCP(void* buffer, Package& res_package) {
    tcphdr* package = (tcphdr*)buffer;

    res_package.src_port = ntohs(package->source);
    res_package.dst_port = ntohs(package->dest);

    printf("src port: %u\n", res_package.src_port);
    printf("dst port: %u\n", res_package.dst_port);
}

// return true, if package should go
//        false if package shouldn't go
bool checkRule(Package res_package, FilterRule rule) {
    // printf("Start checkRule\n");
    // printf("res_package:\n");
    // printPackage(res_package);

    // printf("rule:\n");
    // printPackage(rule.mask);

    bool follow_rule = true;
    if (rule.mask.dst_ip != kNotStated && res_package.dst_ip != rule.mask.dst_ip)
        follow_rule = false;
    if (rule.mask.src_ip != kNotStated && res_package.src_ip != rule.mask.src_ip)
        follow_rule = false;

    if (rule.mask.protocol != IPProtocolType::BROCKEN &&
        rule.mask.protocol != res_package.protocol)
        follow_rule = false;

    if (res_package.protocol == IPProtocolType::TCP ||
        res_package.protocol == IPProtocolType::UDP) {
        if (rule.mask.dst_port != kNotStated && 
            res_package.dst_port != rule.mask.dst_port)
            follow_rule = false;
        if (rule.mask.src_port != kNotStated && 
            res_package.src_port != rule.mask.src_port)
            follow_rule = false;
    }

    // printf("follow_rule = %d\n", follow_rule);
    if (rule.type == RuleType::PASS)
        return follow_rule;
    if (rule.type == RuleType::DELETE)
        return !follow_rule;
    
    printf("ERROR: unknown rule.type(%d)!!!!\n", rule.type);
    return false; 
}

bool processIPv4(void* buffer, FilterRule rule) {
    iphdr*         package  = (iphdr*)buffer;
    IPProtocolType protocol = (IPProtocolType)package->protocol;

    Package res_package;
    res_package.src_ip = ntohl(package->saddr);
    res_package.dst_ip = ntohl(package->daddr);
    res_package.protocol = protocol;

    printf("src: %x\n", res_package.src_ip);
    printf("dst: %x\n", res_package.dst_ip);
    printf("protocol: ");
    
    switch (protocol) {
        case IPProtocolType::ICMP: {
            printf("ICMP\n");
            break;
        }
        case IPProtocolType::TCP: {
            printf("TCP\n");
            processTCP((char*)buffer + sizeof(iphdr), res_package);
            break;
        }
        case IPProtocolType::UDP: {
            printf("UDP\n");
            processUDP((char*)buffer + sizeof(iphdr), res_package);
            break;
        }
        case IPProtocolType::STP: {
            printf("STP\n");
            break;
        }
    
        default:{
            printf("Something other: %d\n", package->protocol);
            break;
        }
    }

    return checkRule(res_package, rule);
}