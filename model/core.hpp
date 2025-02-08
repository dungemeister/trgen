#ifndef TRGEN_CORE_HPP

#define TRGEN_CORE_HPP

#include <stdint.h>
#include <netinet/ip_icmp.h>
#include <iostream>
#include <vector>

struct packet{
    uint8_t *data;
    int status;

};
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

class Pinger{
public:
    
    Pinger(std::string addr, std::vector<std::string> params): m_addr(addr), m_params(params) {}
    ~Pinger() {}

    void ping();
    void stop();
    static void help() {
        std::cout << "*****PINGER*****\n";
        std::cout << "ping [addr] (options) \n";
        std::cout << "Options:\n";
        std::cout << "-c - packets count\n";
        std::cout << "-i - time interval between packets\n";
        std::cout << "-t - TOS value for IP packet\n";
        std::cout << "****************\n";

    };
private:
    std::string m_addr;
    std::vector<std::string> m_params;

};

class Core{
public:
    
    Core() {}
    ~Core() {}
    void ping();
    void traceroute();
    void get_ifaces();
    void generate_traffic();
};
#endif //TRGEN_CORE_HPP