#ifndef TRGEN_PINGER_HPP
#define TRGEN_PINGER_HPP

#include <stdint.h>
#include <netinet/ip_icmp.h>
#include <string>
#include <vector>
#include <iostream>
#include <netdb.h>
#include "trgen_types.hpp"
#include "pingStatistics.hpp"

struct icmp_pkt{
    struct icmphdr header;
    uint8_t data[64 - sizeof(icmphdr)];
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

const static kernel_release LINUX_ICMP_SOCKET_CAPABILITIES_RELEASE = {.kernel = 2,
                                                               .major = 6,
                                                               .minor = 39,
                                                               .patch = 0}; //ICMP socket capabilities linux


class Pinger{
public:
    
    Pinger(std::string addr, std::vector<std::string> params): m_dst_addr(addr), m_pingStat() { parseParams(params); }
    Pinger(kernel_release &ker, std::vector<std::string> params): m_kernel_release(ker), m_pingStat() { parseParams(params); }
    Pinger(): m_kernel_release{}, m_pingStat() {}
    ~Pinger() {}

    static void description() {
    std::cout << "ping - send and receive icmp echo requests with optional settings and data\n";
}

    void parseParams(std::vector<std::string> params);
    void showParams();
    void ping();
    void pingRawSocket();
    void pingUdpIcmp();
    bool resolveHostname(std::string hostname, struct addrinfo *res);
    void setSocketOptions(int socketFd);
    void stop();
    static void help() {
        std::cout << "*****PINGER*****\n";
        std::cout << "ping <addr> <options> \n";
        std::cout << "List of options:\n";
        std::cout << "-c <count> - Count of icmp echo requests\n";
        std::cout << "-i <interval> - time interval between packets\n";
        std::cout << "-Q <TOS> - TOS value for IP packet\n";
        std::cout << "-b <ip_address> - bind source address to <ip_address>\n";
        std::cout << "****************\n";

    };
private:
    kernel_release m_kernel_release;
    

    std::string m_dst_addr;
    std::string m_dst_net_addr;
    std::string m_bind_addr;
    std::string m_bind_interface;
    int m_count = 5;
    int m_count_left = m_count;
    int m_ttl = 64;
    long int m_tos = 0;
    long int m_packet_size = 64;
    timeval m_timeout = {.tv_sec = 1, .tv_usec = 0};
    PingStatistics m_pingStat;
    uint16_t packet_checksum(icmp_pkt &packet)
    {
        uint32_t res = 0;
        uint16_t *ptr = reinterpret_cast<uint16_t*>(&packet);
        int len = sizeof(icmp_pkt);
        //checksum part for header
        for(int i = 0; len > 1; len -= 2)
        {
            res += *ptr;
            ptr++;
        }
        if(len == 1)
        {
            res += *(uint8_t *)ptr;
        }

        while (res >> 16) {
            res = (res & 0xFFFF) + (res >> 16);
        }
        return (~res) & 0xFFFF;
    }

    void print_icmp_type(int type)
    {
        if(type == ICMP_ECHO)
            std:: cout << "ICMP Echo\n";
        else if(type == ICMP_ECHOREPLY)
            std:: cout << "ICMP Echoreply\n";
        else if(type == ICMP_DEST_UNREACH)
            std:: cout << "ICMP Destination ureachable\n";
        else 
            std::cout << type << "\n";
    }
};

#endif //TRGEN_PINGER_HPP