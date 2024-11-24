#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#include "ipv4.h"

const size_t kSize = 1000;

int createSockets(int intf_ind);

void filter(int in_socket, int out_socket);

void startTwoWayFiltration(int in_intf, int out_intf);

int main(int argc, char** argv) {
    std::cout << "Start program\n";

    if (argc != 3) {
        std::cout << "Error number of arguments";
        return -1;
    }

    int in_intf  = atoi(argv[1]);       // input interface index
    int out_intf = atoi(argv[2]);       // output interface index
    std::cout << "in interface index = "  << in_intf  << "\n";
    std::cout << "out interface index = " << out_intf << "\n";
    
    startTwoWayFiltration(in_intf, out_intf);
}

// Fork proccess and start two-way filtration 
void startTwoWayFiltration(int in_intf, int out_intf) {
    int in_sock  = createSockets(in_intf);
    int out_sock = createSockets(out_intf);
    // filter(in_sock, out_sock);

    pid_t pid = fork();
    switch(pid)
    {
        case -1:
            perror("fork");
            exit(-1);
        case 0:
            filter(in_sock, out_sock);
            break;
        default:
            filter(out_sock, in_sock);
            break;
    }
}

// Create raw socket with given interface index 
int createSockets(int intf_ind) {
    std::cout << "\nErrno before socket create: " << strerror(errno) << "\n";
    int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    std::cout << "Errno after socket create: " << strerror(errno) << "\n";

	struct sockaddr_ll addr;
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	addr.sll_ifindex = intf_ind;
	bind(s, (sockaddr*)&addr, sizeof(addr));
    std::cout << "Errno after socket bind: " << strerror(errno) << "\n\n";

    return s;
}

enum EthProtocolType {
    IPv4 = 0x0800,
    IPv6 = 0x86dd,
    ARP  = 0x0806
};

// Function, that filter all traffic, going through given raw socket 
void filter(int in_socket, int out_socket) {

    char buffer[kSize] = {};
    while (true) {
		ssize_t size = read(in_socket, buffer, kSize);
        if (size == -1) {
            std::cout << strerror(errno) << "\n";
            return;
        }
        std::cout << "size = " << size << "\n";
		printf("%02x:%02x:%02x:%02x:%02x:%02x --> %02x:%02x:%02x:%02x:%02x:%02x %02x:%02x\n",
			    buffer[6],  buffer[7], buffer[8], buffer[9], buffer[10], buffer[11],
			    buffer[0],  buffer[1], buffer[2], buffer[3], buffer[4],  buffer[5],
			    buffer[12], buffer[13]);
        
        EthProtocolType protocol = (EthProtocolType)(buffer[12]*256 + buffer[13]);

        switch (protocol)
        {
            case IPv4:
                processIPv4(buffer + sizeof(ether_header));
                printf("IPv4\n");
                break;
            case IPv6:
                printf("IPv6\n");
                break;
            
            case ARP:
                printf("ARP\n");
                break;

            default:
                printf("Something other\n");
                break;
        }

        write(out_socket, buffer, size);
	}
}