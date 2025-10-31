#include "pinger.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <ctime>
#include <sstream>
#include <sys/time.h>
#include <chrono>
#include <thread>
#include <ifaddrs.h>
#include <arpa/inet.h>

void Pinger::parseParams( std::vector<std::string> params) {
    for(int i = 0; i < params.size(); i++)
    {
        if(params[i] == "-c")
        {
            m_count = std::stod(params[i+1].c_str());
            i++;
        }
        else if(params[i] == "-t")
        {
            m_ttl = std::stod(params[i+1].c_str());
            i++;
        }
        else if(params[i] == "-p")
        {
            m_packet_size = std::stod(params[i+1].c_str());
            i++;
        }
        else if(params[i] == "-b")
        {
            m_bind_addr = params[i+1];
            i++;
        }
        else if(params[i] == "-i")
        {
            m_interval_us = std::stod(params[i+1]);
            i++;
        }
        else if(params[i] == "-Q")
        {
            if(params[i+1].find("0x") != std::string::npos)
            {
                m_tos = std::stol(params[i+1].c_str(), NULL, 16);
            }
            else
            {
                m_tos = std::stol(params[i+1].c_str(), NULL, 10);
            }
            i++;
        }
        else
        {
            if(params[i] != "help")
                m_dst_addr = params[i];
            else
                m_default_payload = false;

        }
    }

}

void Pinger::showParams() {
    std::cout << " Dest address " << m_dst_addr;
    std::cout << " (" << m_dst_net_addr << ")";
    std::cout << " Count " << m_count;
    std::cout << " Interval " << m_interval_us;
    std::cout << " TOS 0x" << m_tos;
    std::cout << " TTL " << m_ttl;
    std::cout << " ICMP packet size " << m_packet_size << "\n";
}

void Pinger::payloadRun() {
    if(!m_default_payload) {
        help();
        return;
    }

    if(m_kernel_release >= LINUX_ICMP_SOCKET_CAPABILITIES_RELEASE) {
        // pingUdpIcmp();
        pingRawSocket();

    }
    else {
        if(setuid(0) != 0) {
            perror(" ERROR: Yot need root privilagies to run with kernel without icmp socket capabilities\n");
            exit(static_cast<int>(ExitTypes::NEEDED_ROOT_PRIVILAGES));
        }
        pingRawSocket();
    }
    m_pingStat.showStatistics(m_dst_addr);

}

std::string Pinger::updateView(std::string str){
    return "PINGER: " + str;
}

void Pinger::pingRawSocket() {
    icmp_pkt packet = {};
    sockaddr_in remote_addr;
    int socketFd;
    uint8_t read_buf[m_packet_size];
    int n = 0;
    std::unique_ptr<addrinfo> res;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(0);

    if(!resolveHostname(m_dst_addr, res.get())) {
        return;
    }
    if(!resolveBindAddress(m_bind_addr)) {
        return;
    }

    // if(res != nullptr)
    //     freeaddrinfo(res);

    inet_pton(AF_INET, m_dst_net_addr.c_str(), &remote_addr.sin_addr);

    socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socketFd < 0) {
        
        // perror("ERROR: create RAW socket\n");
        sendFormatedStringToObservers("ERROR: create RAW socket\n %s", strerror(errno));
        // return;
    }
    setSocketOptions(socketFd);

    if(!bindSocketSourceAddr(socketFd, m_bind_addr)) {
        return;
    }
    showParams();

    packet.header.type = ICMP_ECHO;
    packet.header.code = 0;
    packet.header.un.echo.id = htons(getpid() & 0xFFFF);

    auto startTime = std::chrono::steady_clock::now();
    for(int i = 1; i <= m_count; i++) {

        auto now = std::chrono::steady_clock::now();
        auto intervalTime = now + std::chrono::seconds(m_interval_us);
        
        struct timeval sentTime;
        gettimeofday(&sentTime, NULL);
        memcpy(packet.data, &sentTime, sizeof(sentTime));
        packet.header.un.echo.sequence = htons(i);
        packet.header.checksum = icmpPacketChecksum(packet);

        if(sendPacket(socketFd, remote_addr, packet))
        {
            receivePacket(socketFd, read_buf, sentTime);
        }
        else {
            break;
        }
        
        if(i != m_count)
            std::this_thread::sleep_until(intervalTime);
    }
    
    auto execTime = std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
    m_pingStat.setExecTime(execTime);
    close(socketFd);
    return;
}

bool Pinger::sendPacket(int& socketFd, sockaddr_in& remote_addr, icmp_pkt& packet) {
    int n = 0;

    // std::cout <<std::hex << "time sec " << sentTime.tv_sec << " time usec " << sentTime.tv_usec << "\n";

    if( (n = sendto(socketFd, &packet, m_packet_size, 0, 
                    reinterpret_cast<sockaddr*>(&remote_addr), sizeof(remote_addr))) <= 0 ) {
        // perror("ERROR: send icmp packet\n");
        sendFormatedStringToObservers("ERROR: send icmp packet\n%s", strerror(errno));
        return false;
    }
    else {
        char out[128];
        // sprintf(out, "Sended %d bytes to %s ",n, inet_ntoa(remote_addr.sin_addr));
        // sendToObservers(out);
        sendFormatedStringToObservers("Sended %d bytes to %s ",n, inet_ntoa(remote_addr.sin_addr));

        // std::cout << "Sended " << n << " bytes to " << inet_ntoa(remote_addr.sin_addr) << " ";
        m_pingStat.increaseSendedPackets();
    }
    return true;
}

bool Pinger::receivePacket(int& socketFd, uint8_t* read_buf, timeval& sentTime) {
    sockaddr_in recv_addr;
    uint recv_addr_len = sizeof(recv_addr);
    int n = 0;
    double rtt = 0.0;
    struct timeval currentTime;
    icmp_pkt *response_packet = nullptr;

    if ((n = recvfrom(socketFd, static_cast<void*>(read_buf), m_packet_size, 0,
                (sockaddr*)&recv_addr, &recv_addr_len)) <= 0) {
        sendFormatedStringToObservers("Packet receive timeout\n");
    } 
    else {
        gettimeofday(&currentTime, nullptr);
        response_packet = reinterpret_cast<icmp_pkt*>(&read_buf[sizeof(iphdr)]);
        rtt = (currentTime.tv_sec - sentTime.tv_sec) * 1000.0 +
            (currentTime.tv_usec - sentTime.tv_usec) / 1000.0;
        sendFormatedStringToObservers("Packet from %s received. Size %d bytes. rtt %.3lfms ",
        inet_ntoa(recv_addr.sin_addr), n, rtt);

        m_pingStat.increaseReceivedPackets();
        m_pingStat.setMaxRtt(rtt);
        m_pingStat.setMinRtt(rtt);
        printIcmpType(response_packet->header.type);
    }
    return true;
}

bool Pinger::bindSocketSourceAddr(int& socketFD, std::string srcAddr) {
    if (!srcAddr.empty()) {
        struct sockaddr_in sourceAddr;
        sourceAddr.sin_family = AF_INET;
        sourceAddr.sin_addr.s_addr = inet_addr(srcAddr.c_str());
        if (bind(socketFD, reinterpret_cast<sockaddr*>(&sourceAddr), sizeof(sourceAddr)) < 0) {
            sendFormatedStringToObservers(" ERROR: bind source address\n%s", strerror(errno));
            return false;
        }
    }
    return true;
}

void Pinger::pingUdpIcmp() {
    int udpSocket;
    int icmpSocket;
    struct sockaddr_in remoteAddr = {};

    if((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        sendFormatedStringToObservers(" ERROR: fail to open udp socket\n%s", strerror(errno));
        return;
    }
    if((icmpSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        close(udpSocket);
        sendFormatedStringToObservers(" ERROR: fail to open icmp raw socket\n%s", strerror(errno));
        return;
    }

    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(0);

    if(inet_pton(AF_INET, m_dst_addr.c_str(), &remoteAddr.sin_addr) <= 0) {
        sendFormatedStringToObservers(" ERROR: Invalid address\n%s", strerror(errno));
        close(udpSocket);
        close(icmpSocket);
    }
    

    close(udpSocket);
    close(icmpSocket);
}

bool Pinger::resolveHostname(std::string hostname, addrinfo* res) {
    struct addrinfo hints = {};
    int status;
    if((status = getaddrinfo(hostname.c_str(), nullptr, &hints, &res)) != 0) {
        char err[128];
        sendFormatedStringToObservers("Error resolving hostname %s\n", gai_strerror(status));
        return false;
    }
    for(addrinfo *p = res; p != nullptr; p = p->ai_next) {
        void *addr;
        char ipstr[INET6_ADDRSTRLEN];
        if(p->ai_family == AF_INET) {
            sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in*>(p->ai_addr);
            addr = &ipv4->sin_addr;
            inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
            m_dst_net_addr = ipstr;
        }

    }
    return true;
}

bool Pinger::resolveBindAddress(std::string address) {
    //TODO: implement functionality
    if(m_bind_addr != "") {
        ifaddrs *ifAddrs = nullptr;
        ifaddrs *ifAddrsPtr = nullptr;
        if(getifaddrs(&ifAddrs)) {
            sendFormatedStringToObservers(" ERROR: get interfaces addresses %s\n", strerror(errno));
            return false;
        }

        for(ifAddrsPtr = ifAddrs; ifAddrsPtr != nullptr; ifAddrsPtr = ifAddrsPtr->ifa_next) {
            if(ifAddrsPtr->ifa_addr == nullptr) continue;

            if(ifAddrsPtr->ifa_addr->sa_family == AF_INET || ifAddrsPtr->ifa_addr->sa_family == AF_INET6) {
                char addrBuffer[INET6_ADDRSTRLEN];
                void *addrPtr;

                if(ifAddrsPtr->ifa_addr->sa_family == AF_INET) {
                    addrPtr = &(reinterpret_cast<sockaddr_in*>(ifAddrsPtr->ifa_addr))->sin_addr;
                }
                if(ifAddrsPtr->ifa_addr->sa_family == AF_INET6) {
                    addrPtr = &(reinterpret_cast<sockaddr_in6*>(ifAddrsPtr->ifa_addr))->sin6_addr;
                }
                 inet_ntop(ifAddrsPtr->ifa_addr->sa_family, addrPtr, addrBuffer, sizeof(addrBuffer));
                if(m_bind_addr == addrBuffer) {
                    sendFormatedStringToObservers("%s on iface %s\n",addrBuffer, ifAddrsPtr->ifa_name);
                    freeifaddrs(ifAddrs);
                    return true;
                }
            }   
        }
    }
    return true;
}

void Pinger::setSocketOptions(int socketFd) {
    if(setsockopt(socketFd, SOL_IP, IP_TTL, static_cast<void *>(&m_ttl), sizeof(m_ttl))) {
        sendFormatedStringToObservers("ERROR: set socket IP TTL\n");

        // return;
    }
    if(setsockopt(socketFd, SOL_IP, IP_TOS, static_cast<void *>(&m_tos), sizeof(m_tos))) {
        sendFormatedStringToObservers("ERROR: set socket IP TOS\n");

        // return;
    }
    if(setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, static_cast<void *>(&m_timeout), sizeof(m_timeout))) {
        sendFormatedStringToObservers("ERROR: set socket receive timeout\n");
        // return;
    }
}
