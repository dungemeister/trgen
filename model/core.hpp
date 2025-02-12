#ifndef TRGEN_CORE_HPP

#define TRGEN_CORE_HPP

#include <stdint.h>
#include <netinet/ip_icmp.h>
#include <iostream>
#include <sys/utsname.h>
#include <vector>
#include "trgen_types.hpp"


class Core{
public:
    Core() { get_kernel_release(); }
    ~Core() {}
    
    void run(Task& task);

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