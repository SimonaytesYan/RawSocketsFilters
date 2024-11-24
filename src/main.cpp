#include "filter.h"

#include <iostream>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int createSockets(int intf_ind);
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

    if (in_sock == -1 || out_sock == -1) {
        std::cout << "Error during creating sockets\n";
        return;
    }

    Package rule_package = {kNotStated, kNotStated, 80, 80, 
                            IPProtocolType::BROCKEN};

    FilterRule rule = {rule_package, RuleType::PASS};
    std::vector<FilterRule> rules;
    rules.push_back(rule);

    FilterList list = {rules, FilterListType::BLACK_LIST};

    pid_t pid = fork();
    switch(pid)
    {
        case -1:
            perror("fork");
            exit(-1);
        case 0:
            filter(in_sock, out_sock, list);
            break;
        default:
            filter(out_sock, in_sock, list);
            break;
    }
}

// Create raw socket with given interface index 
int createSockets(int intf_ind) {
    int s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	struct sockaddr_ll addr;
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL);
	addr.sll_ifindex = intf_ind;
	bind(s, (sockaddr*)&addr, sizeof(addr));

    if (errno != 0) {
        std::cout << "Errno after socket bind: " << strerror(errno) << "\n\n";
        return -1;
    }

    return s;
}
