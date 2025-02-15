#include <ifaddrs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <iostream>
#include "ifacesList.hpp"

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
    std::cout << "*****IFLIST*****\n";
    std::cout << "iflist <options>\n";
    std::cout << "List of options:\n";
    std::cout << "-n - set required netns name (default value: default)\n";
    std::cout << "****************\n";
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
        perror((" ERROR: open netns " + netnsName).c_str());
        // return false;
    }
    
    auto curNetnsFd = setns(netnsFd, CLONE_NEWNET);
    if(curNetnsFd < 0) {
        perror((" ERROR: switch to netns " + netnsName).c_str());
        // return false;
    }

    if(getifaddrs(&ifAddrs) < 0) {
        perror(" ERROR: get network interfaces\n");
        return false;
    }

    for(ifAddrPtr = ifAddrs; ifAddrPtr != nullptr; ifAddrPtr = ifAddrPtr->ifa_next) {
        if(ifAddrPtr->ifa_addr == nullptr) continue;
        if(ifAddrPtr->ifa_addr->sa_family == AF_INET || ifAddrPtr->ifa_addr->sa_family == AF_INET6 ) {
            ifaceInfo iface = {};
            char addrBuf[INET6_ADDRSTRLEN];
            void *addrPtr;
            
            if(ifAddrPtr->ifa_addr->sa_family == AF_INET) {
                addrPtr = &(reinterpret_cast<sockaddr_in*>(ifAddrPtr->ifa_addr))->sin_addr;
            }
            if(ifAddrPtr->ifa_addr->sa_family == AF_INET6) {
                addrPtr = &(reinterpret_cast<sockaddr_in6*>(ifAddrPtr->ifa_addr))->sin6_addr;
            }
            inet_ntop(ifAddrPtr->ifa_addr->sa_family, addrPtr, addrBuf, sizeof(addrBuf));
            iface.ipAddr = addrBuf;
            iface.name = ifAddrPtr->ifa_name;
            iface.nsName = netnsName;
            // ifAddrPtr->ifa_ifu
            m_ifaces.push_back(iface);
        }
    }
    freeifaddrs(ifAddrs);
    return true;
}

void IfacesList::showIfaces() {
    for(auto iface: m_ifaces) {
        showIface(iface);
    }
}

void IfacesList::showIface(ifaceInfo& iface) {
        std::cout << iface.name << "\t" << iface.macAddr << "\t" << iface.ipAddr <<
        "\t" << iface.nsName << "\t" << iface.state << "\n";
}