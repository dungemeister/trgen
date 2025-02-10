#ifndef TRGEN_CORE_HPP

#define TRGEN_CORE_HPP

#include <stdint.h>
#include <netinet/ip_icmp.h>
#include <iostream>
#include <sys/utsname.h>
#include <vector>
#include "pinger.hpp"
#include "trgen_types.hpp"


class Core{
public:
    
    Core() { get_kernel_release(); }
    ~Core() {}
    
    static void help() {
        Pinger::description();
    }
    
    void ping(std::vector<std::string> params);
    void traceroute();
    void get_ifaces();
    void generate_traffic();
private:
    kernel_release m_kernel_release = {0};

    void get_kernel_release() {
        struct utsname buffer = {};
        
        if(uname(&buffer)) {
            perror(" ERROR:uname\n");
            return;
        }
        std::cout << "Kernel release " << buffer.release << "\n";
        char *p = reinterpret_cast<char*>(buffer.release);
        int *k = reinterpret_cast<int *>(&m_kernel_release);
        int i = 0;
        while (*p) {
            if (isdigit(*p)) {
                k[i] = strtol(p, &p, 10);
                i++;
            } else {
                p++;
            }
        }
    }    
};
#endif //TRGEN_CORE_HPP