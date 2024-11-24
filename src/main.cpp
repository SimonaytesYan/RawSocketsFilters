#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

const size_t kSize = 1000;

int createSockets(int intf_ind);

void filter(int socket);

int main(int argc, char** argv) {
    std::cout << "Start program\n";

    if (argc != 2) {
        std::cout << "Error number of arguments";
        return -1;
    }

    int intf_ind = atoi(argv[1]);       // interface index
    std::cout << "interface index = " << intf_ind << "\n";

    int s1 = createSockets(intf_ind);
    filter(s1);
}

// Create raw socket with given interface index 
int createSockets(int intf_ind) {
    std::cout << "Errno before socket create: " << strerror(errno) << "\n";
    int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    std::cout << "Errno after socket create: " << strerror(errno) << "\n";

	struct sockaddr_ll addr;
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	addr.sll_ifindex = intf_ind;
	bind(s, (sockaddr*)&addr, sizeof(addr));
    std::cout << "Errno after socket bind: " << strerror(errno) << "\n";

    return s;
}

// Function, that filter all traffic, going through given raw socket 
void filter(int socket) {

    char buffer[kSize] = {};
    while (true) {
		ssize_t size = read(socket, buffer, kSize);
        if (size == -1) {
            std::cout << strerror(errno) << "\n";
            return;
        }
        std::cout << "size = " << size << "\n";
		printf("%02x:%02x:%02x:%02x:%02x:%02x --> %02x:%02x:%02x:%02x:%02x:%02x %02x:%02x\n",
			    buffer[6],  buffer[7], buffer[8], buffer[9], buffer[10], buffer[11],
			    buffer[0],  buffer[1], buffer[2], buffer[3], buffer[4],  buffer[5],
			    buffer[12], buffer[13]);
	}
}