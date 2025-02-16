#ifndef TRGEN_IFACES_LIST_HPP
#define TRGEN_IFACES_LIST_HPP

#include <iostream>
#include "payload.hpp"
#include "trgenTypes.hpp"
#include <unordered_map>

#ifndef _NETNS_IFACES_FROM_IP_UTIL
#define _NETNS_IFACES_FROM_IP_UTIL
#else
#define _NETNS_IFACES_FROM_GETIFADDRS
#endif

class IfacesList: public Payload {
public:    
    IfacesList(std::vector<std::string>& params): m_ifaces() { parseParams(params); }
    ~IfacesList() {}
    void payloadRun() override;
    void help() override;

    static void description() {
        std::cout << "iflist - list all ifaces in active netns\n";
    }
private:
    std::unordered_map<std::string, ifaceInfo> m_ifaces;
    std::string m_curNetns = "default";
    bool m_default_payload = true;

    void parseParams(std::vector<std::string>& params);
    bool parseIfaces(const std::string& netnsName);
    void showIfaces();
    void showIface(ifaceInfo& iface);
    void macToCharBuf(unsigned char *sll_addr, char *buf, size_t len);
    std::string convertNetmask(uint32_t addr);
};

#endif //TRGEN_IFACES_LIST_HPP