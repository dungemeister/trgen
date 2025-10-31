#ifndef TRGEN_PINGER_HPP
#define TRGEN_PINGER_HPP

#include <stdint.h>
#include <netinet/ip_icmp.h>
#include <string>
#include <vector>
#include <iostream>
#include <netdb.h>
#include "subject.hpp"
#include "trgenTypes.hpp"
#include "pingStatistics.hpp"
#include "payload.hpp"

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

const static KernelRelease LINUX_ICMP_SOCKET_CAPABILITIES_RELEASE = {.kernel = 2,
                                                               .major = 6,
                                                               .minor = 39,
                                                               .patch = 0}; //ICMP socket capabilities linux

const int DEFAULT_PING_COUNT = 5;
class Pinger: public Payload{
public:
    
    Pinger(KernelRelease &ker, std::vector<std::string> params): m_kernel_release(ker), m_pingStat() { parseParams(params); }
    Pinger(): m_kernel_release{}, m_pingStat() {}
    ~Pinger() {}
    
    void payloadRun() override;
    std::string updateView(std::string str);

    static void description() {
    std::cout << "ping - send and receive icmp echo requests with optional settings and data\n";
}

    void help() override {
        std::cout << "*****PINGER*****\n";
        std::cout << "ping <addr> <options> \n";
        std::cout << "List of options:\n";
        std::cout << "-c <count> - Count of icmp echo requests (default value: " << DEFAULT_PING_COUNT << ")\n";
        std::cout << "-i <interval> - time interval between packets (default value: 1.0sec)\n";
        std::cout << "-Q <TOS> - TOS value for IP packet  (default value: 0x0)\n";
        std::cout << "-b <ip_address> - bind source address to <ip_address>\n";
        std::cout << "****************\n";

    } ;
private:
    KernelRelease m_kernel_release;
    
    bool m_default_payload = true;
    std::string m_dst_addr;
    std::string m_dst_net_addr;
    std::string m_bind_addr = "";
    std::string m_bind_interface;
    int m_count = DEFAULT_PING_COUNT;
    int m_count_left = m_count;
    int m_ttl = 64;
    long int m_tos = 0;
    long int m_packet_size = 64;
    timeval m_timeout = {.tv_sec = 1, .tv_usec = 0};
    long m_interval_us = 1;
    PingStatistics m_pingStat;

    void parseParams(std::vector<std::string> params);
    void showParams();
    void pingRawSocket();
    bool sendPacket(int& socketFd, sockaddr_in& remote_addr, icmp_pkt& packet);
    bool receivePacket(int& socketFd, uint8_t* read_buf, timeval& sentTime);
    bool bindSocketSourceAddr(int& socketFD, std::string srcAddr);
    void pingUdpIcmp();
    bool resolveHostname(std::string hostname, addrinfo* res);
    bool resolveBindAddress(std::string address);
    void setSocketOptions(int socketFd);

    uint16_t icmpPacketChecksum(icmp_pkt &packet)
    {
        uint32_t sum = 0;
        auto size = sizeof(icmp_pkt);
        uint8_t* buffer = reinterpret_cast<uint8_t*>(&packet);

        // Суммируем 16-битные слова
        for (size_t i = 0; i < size; i += 2) {
            if (i + 1 < size) {
                sum += (static_cast<uint16_t>(buffer[i]) << 8) | buffer[i + 1];
            } else {
                // Обработка нечётного байта
                sum += static_cast<uint16_t>(buffer[i]) << 8;
            }
        }

        // Складываем переносы
        while (sum >> 16) {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }

        // Инвертируем результат
        return static_cast<uint16_t>(~sum);
    }

    void printIcmpType(int type)
    {
        if(type == ICMP_ECHO)
            sendFormatedStringToObservers("ICMP Echo\n");
        else if(type == ICMP_ECHOREPLY)
            sendFormatedStringToObservers("ICMP Echoreply\n");
        else if(type == ICMP_DEST_UNREACH)
            sendFormatedStringToObservers("ICMP Destination ureachable\n");
        else 
            sendFormatedStringToObservers("%d \n", type);
    }
};

#endif //TRGEN_PINGER_HPP