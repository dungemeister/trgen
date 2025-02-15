#ifndef TRGEN_IFACES_LIST_HPP
#define TRGEN_IFACES_LIST_HPP

#include <iostream>
#include "payload.hpp"
#include "trgenTypes.hpp"

#ifndef _NETNS_IFACES_FROM_IP_UTIL
#define _NETNS_IFACES_FROM_IP_UTIL
#else
#define _NETNS_IFACES_FROM_GETIFADDRS
#endif

class IfacesList: public Payload {
public:    
    IfacesList(): m_ifaces() {}
    ~IfacesList() {}
    void payloadRun() override;
    void help() override;

    static void description() {
        std::cout << "iflist - list all ifaces in active netns\n";
    }
private:
    std::vector<ifaceInfo> m_ifaces;
    std::string m_curNetns = "default";

    bool parseIfaces(const std::string& netnsName);
    void showIfaces();
    void showIface(ifaceInfo& iface);
};

#endif //TRGEN_IFACES_LIST_HPP