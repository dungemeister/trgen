#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <iostream>
#include "ifacesList.hpp"
#include <errno.h>
#include <string.h>

void IfacesList::payloadRun() {
    if(m_default_payload) {
        parseIfaces(m_curNetns);
        showIfaces();
    }
    else {
        help();
    }
}

void IfacesList::help() {
    sendFormatedStringToObservers("*****IFLIST*****\n");
    sendFormatedStringToObservers("iflist <options>\n");
    sendFormatedStringToObservers("List of options:\n");
    sendFormatedStringToObservers("-n - set required netns name (default value: default)\n");
    sendFormatedStringToObservers("****************\n");
}

void IfacesList::parseParams(std::vector<std::string>& params){
    for(int i = 0; i < params.size(); i++) {
        if(params[i] == "-n")
        {
            m_curNetns = params[i+1];
            i++;
        }
        else {
            if(params[i] == "help")
                m_default_payload = false;
        }
    }
}

bool IfacesList::parseIfaces(const std::string& netnsName) {
    ifaddrs *ifAddrs = nullptr;
    ifaddrs *ifAddrPtr = nullptr;
    
    int netnsFd = open(("/var/run/netns/" + netnsName).c_str(), O_RDONLY);
    if(netnsFd < 0) {
        sendFormatedStringToObservers(" ERROR: open netns %s\n%s", netnsName, strerror(errno));
        // return false;
    }
    
    auto curNetnsFd = setns(netnsFd, CLONE_NEWNET);
    if(curNetnsFd < 0) {
        sendFormatedStringToObservers(" ERROR: switch to netns  %s\n%s", netnsName, strerror(errno));
        // return false;
    }

    if(getifaddrs(&ifAddrs) < 0) {
        sendFormatedStringToObservers(" ERROR: get network interfaces %s\n%s", netnsName, strerror(errno));
        return false;
    }

    for(ifAddrPtr = ifAddrs; ifAddrPtr != nullptr; ifAddrPtr = ifAddrPtr->ifa_next) {
        if(!ifAddrPtr->ifa_name) continue;
        IfaceInfo iface = {};
        iface.name = ifAddrPtr->ifa_name;
        iface.nsName = netnsName;

        
        iface.state = ifAddrPtr->ifa_flags & IFF_UP;
        if(ifAddrPtr->ifa_addr && ifAddrPtr->ifa_addr->sa_family == AF_PACKET) {
            if(m_ifaces.count(ifAddrPtr->ifa_name) > 0) {
                iface = m_ifaces[ifAddrPtr->ifa_name];
            }
            sockaddr_ll *l2Addr = reinterpret_cast<sockaddr_ll*>(ifAddrPtr->ifa_addr);
            unsigned char *mac = l2Addr->sll_addr;
            size_t bufLen = 18;
            char macBuf[bufLen];
            macToCharBuf(mac, macBuf, bufLen);
            iface.macAddr = macBuf;
        }
        if(ifAddrPtr->ifa_addr && (ifAddrPtr->ifa_addr->sa_family == AF_INET ||
                                   ifAddrPtr->ifa_addr->sa_family == AF_INET6 )) {
            char addrBuf[INET6_ADDRSTRLEN];
            void *addrPtr;
            char netmaskBuf[INET6_ADDRSTRLEN] = {};
            in_addr *ipNetmask;
            if(m_ifaces.count(ifAddrPtr->ifa_name) > 0) {
                iface = m_ifaces[ifAddrPtr->ifa_name];
            }
            if(ifAddrPtr->ifa_addr->sa_family == AF_INET) {
                addrPtr = &(reinterpret_cast<sockaddr_in*>(ifAddrPtr->ifa_addr))->sin_addr;
            }
            if(ifAddrPtr->ifa_addr->sa_family == AF_INET6) {
                addrPtr = &(reinterpret_cast<sockaddr_in6*>(ifAddrPtr->ifa_addr))->sin6_addr;
            }
            inet_ntop(ifAddrPtr->ifa_addr->sa_family, addrPtr, addrBuf, sizeof(addrBuf));
            
            ipNetmask = &reinterpret_cast<sockaddr_in*>(ifAddrPtr->ifa_netmask)->sin_addr;
            inet_ntop(ifAddrPtr->ifa_addr->sa_family, ipNetmask, netmaskBuf, sizeof(netmaskBuf));

            iface.ipAddr.push_back(std::make_pair(addrBuf, convertNetmask(*reinterpret_cast<uint32_t*>(ipNetmask))));

        }
        if(m_ifaces.count(ifAddrPtr->ifa_name) > 0)
            m_ifaces[ifAddrPtr->ifa_name] = iface;
        else
            m_ifaces.insert(std::make_pair(ifAddrPtr->ifa_name, iface));
    }

    setns(curNetnsFd, CLONE_NEWNET);
    freeifaddrs(ifAddrs);
    close(netnsFd);
    return true;
}

void IfacesList::showIfaces() {
    int i = 1;
    for(auto it: m_ifaces) {
        std::cout << i++ << "\t";
        sendFormatedStringToObservers("%d\t", i++);
        showIface(it.second);
    }
}

void IfacesList::showIface(IfaceInfo& iface) {
        std::cout << iface.name << "\t" << iface.macAddr << "\t";
        for(auto ip: iface.ipAddr) {
            sendFormatedStringToObservers("%s/%s ", ip.first, ip.second);
        }
        sendFormatedStringToObservers("\t%s\t%s\n", iface.nsName, iface.state);
}

void IfacesList::macToCharBuf(unsigned char *sll_addr, char *buf, size_t len){
                snprintf(buf, len, "%02x:%02x:%02x:%02x:%02x:%02x", sll_addr[0],
            sll_addr[1], sll_addr[2], sll_addr[3], sll_addr[4], sll_addr[5]);
}

std::string IfacesList::convertNetmask(uint32_t addr){
    //Each byte in addr needs to be shifted in reverse
    int i = 0;
    int bytes = 0;
    for(int j = 0; j < sizeof(uint32_t); j++) {
        uint8_t byte = addr & 0xFF;
        while(byte > 0) {
            byte = byte << 1;
            i++;
        }
        addr = addr >> 8;
    }
    return std::string("/" + std::to_string(i));
}