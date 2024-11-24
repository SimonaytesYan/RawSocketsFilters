#include "filter.h"
#include "ipv4.h"

#include <errno.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const size_t kSize = 1000;

enum class EthProtocolType {
    IPv4 = 0x0800,
    IPv6 = 0x86dd,
    ARP  = 0x0806
};

// Function, that filter all traffic, going through given raw socket 
void filter(int in_socket, int out_socket, const FilterList& list) {

    char buffer[kSize] = {};
    while (true) {
		ssize_t size = read(in_socket, buffer, kSize);

        if (size == -1) {
            printf("%s\n", strerror(errno));
            return;
        }

        printf("size = %zu\n", size);
		printf("%02x:%02x:%02x:%02x:%02x:%02x --> %02x:%02x:%02x:%02x:%02x:%02x %02x:%02x\n",
			    buffer[6],  buffer[7], buffer[8], buffer[9], buffer[10], buffer[11],
			    buffer[0],  buffer[1], buffer[2], buffer[3], buffer[4],  buffer[5],
			    buffer[12], buffer[13]);
        
        EthProtocolType protocol = (EthProtocolType)(buffer[12]*256 + buffer[13]);

        bool package_follow_rule = true; 
        switch (protocol)
        {
            case EthProtocolType::IPv4:
                package_follow_rule = processIPv4(buffer + sizeof(ether_header), rule);
                printf("IPv4\n");
                break;
            case EthProtocolType::IPv6:
                printf("IPv6\n");
                break;
            
            case EthProtocolType::ARP:
                printf("ARP\n");
                break;

            default:
                printf("Something other\n");
                break;
        }
        
        if (package_follow_rule) {
            write(out_socket, buffer, size);
            printf("\n \033[1;32mPackage passed\033[0m\n\n");
        }
        else
            printf("\n \033[1;31mPackage deleted\033[0m\n\n");
	}
}

void printPackage(Package pkg) {
    printf("{\n");
    printf("\tdst = 0x%x:%d\n", pkg.dst_ip, pkg.dst_port);
    printf("\tsrc = 0x%x:%d\n", pkg.src_ip, pkg.src_port);
    printf("\tprt = %d\n", pkg.protocol);
    printf("}\n");
}
