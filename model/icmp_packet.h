#ifndef _ICMP_PACKET_H
#define _ICMP_PACKET_H

#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define DEFAULT_DST_IP_ADDRESS "127.0.0.1"

typedef struct icmp_pkt icmp_pkt;

struct icmp_pkt{
    struct icmphdr header;
    char data[64 - sizeof(icmphdr)];
};

typedef struct ping_params ping_params;
struct ping_params{
    char *dst_addr;
    char *bind_addr;
    char *bind_interface;
    int count;
    int ttl;
    int tos;

};

void ping(int argc, char** argv);

#ifdef ICMP_PACKET_IMPLEMENTATION

uint16_t checksum(void *buf, size_t len)
{
    uint32_t res = 0;
    uint16_t *b = (uint16_t*)buf;
    for(int i = 0; len > 1; len -= 2)
    {
        res += *b;
        b++;
    }
    if(len == 1)
    {
        res += *(uint8_t *)b;
    }
    res = (res >> 16) + (res & 0xFFFF);
    res += (res >> 16);
    return ~res;
}

void ping_help()
{
    printf("\nUsage: ping <src_addr> <options>");
    printf("\n List of options:");
    printf("\n -c <count>       - Count of icmp echo requests");
    printf("\n -b <ip address>  - Bind source address");
    printf("\n -Q <TOS>         - Set IP type of service class");
    printf("\n -t <TTL>         - Set request TTL");

    printf("\n");
}

void parse_arguments(ping_params *params, int argc, char **argv)
{
    if(argc - 1 < 1)
    {
        ping_help();
        exit(1);
    }
    printf("\n Args count %d", argc);

    for(int i = 0; i < argc; i++)
    {
        if(!strcmp(argv[i], "-c"))
        {
            params->count = atoi(argv[i+1]);
            i++;
        }
        else if(!strcmp(argv[i], "-t"))
        {
            params->ttl = atoi(argv[i+1]);
            i++;
        }
        else if(!strcmp(argv[i], "-Q"))
        {
            if(strncmp(argv[i + 1], "0x", 2) == 0)
            {
                params->tos = strtol(argv[i+1], NULL, 16);
            }
            else
            {
                params->tos = strtol(argv[i+1], NULL, 10);
            }
            i++;
        }
        else
        {
            params->dst_addr = argv[i];

        }
    }
    printf("\n Count %d", params->count);
    printf("\n Dest address %s", params->dst_addr);
    printf("\n TOS 0x%x", params->tos);
    printf("\n TTL %d", params->ttl);
}

void print_icmp_type(int type)
{
    if(type == ICMP_ECHO)
    printf("\n ICMP Echo");
    else if(type == ICMP_ECHOREPLY)
    printf("\n ICMP Echoreply");
    else if(type == ICMP_DEST_UNREACH)
    printf("\n ICMP Destination ureachable");
}

void ping(int argc, char** argv)
{
    ping_params params;
    params.count = 10;
    params.ttl = 64;
    params.tos = IPTOS_CLASS_CS0;
    params.dst_addr = (char *)malloc(sizeof(DEFAULT_DST_IP_ADDRESS));
    strcpy(params.dst_addr, DEFAULT_DST_IP_ADDRESS);

    parse_arguments(&params, argc, argv);
    // in_addr_t src_addr = INADDR_LOOPBACK;
    // in_addr_t dst_addr = INADDR_LOOPBACK;
    
    iphdr *response_ip_hdr;
    icmp_pkt packet = {};
    icmp_pkt *response_icmp = {};
    hostent *hostaddr;
    sockaddr_in remote_addr;
    sockaddr_in r_addr;
    int socket_fd;
    timeval timeout;
    uint16_t msg_cnt = 1;

    timeout.tv_sec = 1; //receiving timeout
    timeout.tv_usec = 0;

    if((hostaddr = gethostbyname("localhost")) == NULL)
    {
        printf("\nERROR: get localhost host params");
        
        perror("error msg - ");
        exit(1);
    }
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(0); //auto port
    // addr.sin_addr.s_addr = *(long*)hostaddr->h_addr_list;  // First way 
    inet_pton(AF_INET, params.dst_addr, &remote_addr.sin_addr);     // Second way

    socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socket_fd < 0)
    {
        printf("\nERROR: set socket FD fail");
        
        perror("error msg - ");
        // exit(1);
    }
    if(setsockopt(socket_fd, SOL_IP, IP_TTL, (uint8_t*)&params.ttl, sizeof(params.ttl)))
    {
        printf("\nERROR: set socket option IP TTL fail");
        // exit(2);
    }
    if(setsockopt(socket_fd, SOL_IP, IP_TOS, (uint8_t*)&params.tos, sizeof(params.tos)))
    {
        printf("\nERROR: set socket option IP TOS fail");
        // exit(3);
    }
    if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (uint8_t*)&timeout, sizeof(timeout)))
    {
        printf("\nERROR: set socket option receive timeout fail");
        // exit(4);
    }
    // printf("\nSUCCESS: options are set");

    // printf("\nPing id - 0x%x", getpid()& 0xFFFF);

    packet.header.type = ICMP_ECHO;
    packet.header.code = 0;
    packet.header.un.echo.id = getpid();
    while(msg_cnt < params.count)
    {
        packet.header.un.echo.sequence = (msg_cnt << 8) + (msg_cnt >> 8);
        strcpy(packet.data, "test");

        packet.header.checksum = checksum(&packet, sizeof(packet));
        msg_cnt++;
        int n = sendto(socket_fd, &packet, sizeof(packet), 0, (sockaddr*)&remote_addr, sizeof(remote_addr));
        printf("\nL3 Bytes send %d", n);

        uint8_t *rbuffer[128] = {};
        uint r_addr_len = sizeof(r_addr);
        if ((n = recvfrom(socket_fd, rbuffer, sizeof(rbuffer), 0,
                    (struct sockaddr*)&r_addr, &r_addr_len)) <= 0) {
            printf("\nPacket receive failed!\n");
        } else {
            printf("\nPacket from %s received! Size %d bytes\n", inet_ntoa(r_addr.sin_addr), n);
            response_ip_hdr = (iphdr *)&rbuffer[0];
            response_icmp = (icmp_pkt *)&rbuffer[sizeof(iphdr)];
            print_icmp_type(response_icmp->header.type);
        }
    }
}
#endif //ICMP_PACKET_IMPLEMENTATION

#endif //_ICMP_PACKET_H