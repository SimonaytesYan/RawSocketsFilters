#include "filter.h"

#include <iostream>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const size_t kMaxBufferSize = 1024;

int createSockets(int intf_ind);
void startTwoWayFiltration(int in_intf, int out_intf, const char* filter_rule_file);

int main(int argc, char** argv) {
    std::cout << "Start program\n";

    if (argc != 4) {
        std::cout << "Error number of arguments";
        return -1;
    }

    int         in_intf          = atoi(argv[1]);  // input interface index
    int         out_intf         = atoi(argv[2]);  // output interface index
    const char* filter_rule_file = argv[3];        // output interface index
    std::cout << "in interface index  = "  << in_intf  << "\n";
    std::cout << "out interface index = " << out_intf << "\n";
    std::cout << "rule path file      = " << filter_rule_file << "\n";
    
    startTwoWayFiltration(in_intf, out_intf, filter_rule_file);
}

void skipSpaces(char** ptr) {
    while(**ptr == ' ') {
        (*ptr)++;
    }

}

void skipToSpace(char** ptr) {
    while(**ptr != ' ' && **ptr != '\n' && **ptr != '\0') {
        (*ptr)++;
    }
}

#define GET_FIELD(field_name, str_to_type)                      \
    if (!strncmp(ptr, #field_name, strlen(#field_name))) {      \
        ptr += strlen(#field_name);                             \
        skipSpaces(&ptr);                                       \
        rule.mask.field_name = str_to_type(ptr);                \
        skipToSpace(&ptr);                                      \
    }

IPProtocolType strToProtocol(const char* str) {
    if (!strncmp(str, "udp", strlen("udp")))
        return IPProtocolType::UDP;
    if (!strncmp(str, "tcp", strlen("tcp")))
        return IPProtocolType::TCP;

    printf("Error during parsing protocol");
    return IPProtocolType::BROCKEN;
}

uint32_t strToIP(const char* str) {
    uint32_t p1 = 0, p2 = 0, p3 = 0, p4 = 0;
    if (sscanf(str, "%d.%d.%d.%d", &p1, &p2, &p3, &p4) != 4) {
        printf("Error during reading IP\n");
        return -1;
    }
    
    uint32_t res = p4 + (p3 << 8) + (p2 << 18) + (p1 << 24);
    return res;
}

FilterList getRulesFromFile(const char* filter_rule_file) {
    FilterList list;

    FILE* fp = fopen(filter_rule_file, "r");

    char buffer[kMaxBufferSize] = {};
    while (true) {
        FilterRule rule = {};

        if (fscanf(fp, " %1023[^\n]", buffer) == 0){
            int c = getc(fp);
            return list;
        }

        char* ptr = buffer;

        if (*ptr == '+')
            rule.type = RuleType::PASS;
        else if (*ptr == '-')
            rule.type = RuleType::DELETE;
        else {
            printf("Type +/- in the start of every line\n");
            return list;
        }
        ptr++;

        if (*ptr == '\0') {
            // Last line processing    
            if (rule.type == RuleType::PASS)
                list.type = FilterListType::WHITE_LIST;
            else
                list.type = FilterListType::BLACK_LIST;

            fclose(fp);
            return list;
        }
        
        while (*ptr != '\n' && *ptr != '\0') {
            // Get rule

            skipSpaces(&ptr);
            GET_FIELD(src_ip, strToIP)
            else GET_FIELD(dst_ip, strToIP)
            else GET_FIELD(src_port, atoi)
            else GET_FIELD(dst_port, atoi)
            else GET_FIELD(protocol, strToProtocol)
            else {
                printf("Error field name!\n");
                fclose(fp);
                return list;
            }
        }

        list.rules.push_back(rule);
    }

    printf("Do not get type of list\n");
    fclose(fp);
    return list;
}

// Fork proccess and start two-way filtration 
void startTwoWayFiltration(int in_intf, int out_intf, const char* filter_rule_file) {

    FilterList list = getRulesFromFile(filter_rule_file);

    int in_sock  = createSockets(in_intf);
    int out_sock = createSockets(out_intf);

    if (in_sock == -1 || out_sock == -1) {
        std::cout << "Error during creating sockets\n";
        return;
    }

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
