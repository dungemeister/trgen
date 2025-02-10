#include "pinger.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <ctime>
#include <sys/time.h>

void Pinger::parseParams( std::vector<std::string> params) {
    for(int i = 0; i < params.size(); i++)
    {
        if(params[i] == "-c")
        {
            m_count = atoi(params[i+1].c_str());
            i++;
        }
        else if(params[i] == "-t")
        {
            m_ttl = atoi(params[i+1].c_str());
            i++;
        }
        else if(params[i] == "-p")
        {
            m_packet_size = atoi(params[i+1].c_str());
            i++;
        }
        else if(params[i] == "-b")
        {
            m_bind_addr = params[i+1];
            i++;
        }
        else if(params[i] == "-Q")
        {
            if(params[i+1].find("0x") != std::string::npos)
            {
                m_tos = strtol(params[i+1].c_str(), NULL, 16);
            }
            else
            {
                m_tos = strtol(params[i+1].c_str(), NULL, 10);
            }
            i++;
        }
        else
        {
            m_dst_addr = params[i];

        }
    }


}

void Pinger::showParams() {
    std::cout << " Dest address " << m_dst_addr;
    std::cout << " (" << m_dst_net_addr << ")";
    std::cout << " Count " << m_count;
    std::cout << " TOS 0x" << m_tos;
    std::cout << " TTL " << m_ttl;
    std::cout << " ICMP packet size " << m_packet_size << "\n";
}

void Pinger::ping() {
    kernel_release k = {};

    if(m_kernel_release >= LINUX_ICMP_SOCKET_CAPABILITIES_RELEASE) {
        // pingUdpIcmp();
        pingRawSocket();

    }
    else {
        if(setuid(0) != 0) {
            perror(" ERROR: Yot need root privilagies to run with kernel without icmp socket capabilities\n");
            exit(static_cast<int>(exit_types::NEEDED_ROOT_PRIVILAGES));
        }
        pingRawSocket();
    }
    
    m_pingStat.showStatistics(m_dst_addr);

}

void Pinger::pingRawSocket() {
    iphdr *response_ip_hdr;
    icmp_pkt packet = {};
    icmp_pkt *response_packet = NULL;
    hostent *hostaddr;
    sockaddr_in remote_addr;
    sockaddr_in recv_addr;
    int socketFd;
    uint8_t read_buf[m_packet_size];
    int n = 0;
    struct addrinfo *res;

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(0);

    if(!resolveHostname(m_dst_addr, res)) {
        return;
    }
    // if(res != nullptr)
    //     freeaddrinfo(res);

    inet_pton(AF_INET, m_dst_net_addr.c_str(), &remote_addr.sin_addr);

    socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socketFd < 0) {
        perror("ERROR: create RAW socket\n");
        // return;
    }
    setSocketOptions(socketFd);

    showParams();

    packet.header.type = ICMP_ECHO;
    packet.header.code = 0;
    packet.header.un.echo.id = htons(getpid() & 0xFFFF);

    clock_t startClock =  clock();
    for(int i = 1; i <= m_count; i++) {
        
        //TODO: set packet data
        struct timeval sentTime;
        gettimeofday(&sentTime, NULL);
        memcpy(packet.data, &sentTime, sizeof(sentTime));

        // std::cout <<std::hex << "time sec " << sentTime.tv_sec << " time usec " << sentTime.tv_usec << "\n";
        packet.header.un.echo.sequence = htons(i);
        packet.header.checksum = packet_checksum(packet);
        if( (n = sendto(socketFd, &packet, m_packet_size, 0, 
                        (sockaddr*)&remote_addr, sizeof(remote_addr))) <= 0 ) {
            perror("ERROR: send icmp packet\n");
            return;
        }
        else {
            std::cout << "Sended " << n << " bytes to " << inet_ntoa(remote_addr.sin_addr) << " ";
            m_pingStat.increaseSendedPackets();
        }
        
        uint recv_addr_len = sizeof(recv_addr);
        double rtt = 0.0;
        struct timeval currentTime;

        gettimeofday(&currentTime, nullptr);
        if ((n = recvfrom(socketFd, read_buf, m_packet_size, 0,
                    (sockaddr*)&recv_addr, &recv_addr_len)) <= 0) {
            std::cout << "Packet receive timeout\n";
        } 
        else {
            gettimeofday(&currentTime, nullptr);
            rtt = (currentTime.tv_sec - sentTime.tv_sec) * 1000.0 +
             (currentTime.tv_usec - sentTime.tv_usec) / 1000.0;
            std::cout << "Packet from " << inet_ntoa(recv_addr.sin_addr) << " received. Size " << n
            << " bytes. rtt = " << rtt << "ms ";
            response_packet = (icmp_pkt*)&read_buf[sizeof(iphdr)];

            m_pingStat.increaseReceivedPackets();
            m_pingStat.setMaxRtt(rtt);
            m_pingStat.setMinRtt(rtt);
            // response_ip_hdr = (iphdr *)&rbuffer[0];
            // response_icmp = (icmp_pkt *)&rbuffer[sizeof(iphdr)];
            // print_icmp_type(response_icmp->header.type);
            print_icmp_type(response_packet->header.type);
        }
        
    }
    clock_t endClock =  clock();
    m_pingStat.setExecTime(static_cast<double>(endClock - startClock) / CLOCKS_PER_SEC * 1000);

    close(socketFd);
    return;
}

void Pinger::pingUdpIcmp() {
    int udpSocket;
    int icmpSocket;
    struct sockaddr_in remoteAddr = {};

    if((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror(" ERROR: fail to open udp socket\n");
        return;
    }
    if((icmpSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        close(udpSocket);
        perror(" ERROR: fail to open icmp raw socket\n");
        return;
    }

    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(0);

    if(inet_pton(AF_INET, m_dst_addr.c_str(), &remoteAddr.sin_addr) <= 0) {

        perror(" ERROR: Invalid address\n");
        close(udpSocket);
        close(icmpSocket);
    }
    

    close(udpSocket);
    close(icmpSocket);
}

bool Pinger::resolveHostname(std::string hostname, addrinfo *res) {
    struct addrinfo hints = {};
    int status;
    if((status = getaddrinfo(hostname.c_str(), nullptr, &hints, &res)) != 0) {
        std::cerr << "Error resolving hostname: " << gai_strerror(status) << std::endl;
        return false;
    }

    for(addrinfo *p = res; p != nullptr; p = p->ai_next) {
        void *addr;
        char ipstr[INET6_ADDRSTRLEN];
        if(p->ai_family == AF_INET) {
            sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in*>(p->ai_addr);
            addr = &ipv4->sin_addr;
            inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
            
            if(m_bind_addr != "") {
                if(m_bind_addr == ipstr) {
                    m_dst_net_addr = ipstr;
                }
            }
            else {
                m_dst_net_addr = ipstr;
            }
        }

    }
    return true;
}

void Pinger::setSocketOptions(int socketFd) {
        if(setsockopt(socketFd, SOL_IP, IP_TTL, (void *)&m_ttl, sizeof(m_ttl))) {
        perror("ERROR: set socket IP TTL\n");
        // return;
    }
    if(setsockopt(socketFd, SOL_IP, IP_TOS, (void *)&m_tos, sizeof(m_tos))) {
        perror("ERROR: set socket IP TOS\n");
        // return;
    }
    if(setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, (void *)&m_timeout, sizeof(m_timeout))) {
        perror("ERROR: set socket receive timeout\n");
        // return;
    }
}