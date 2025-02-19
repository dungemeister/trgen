#ifndef TRGEN_CORE_HPP

#define TRGEN_CORE_HPP

#include <netinet/ip_icmp.h>
#include <iostream>
#include <sys/utsname.h>
#include <vector>
#include <memory>
#include <algorithm>
#include "trgenTypes.hpp"
#include "helper.hpp"
#include "pinger.hpp"
#include "ifacesList.hpp"
#include "observer.hpp"
class Core{
public:
    Core() { getKernelRelease(); }
    ~Core() {}
    
    void run(Task& task);
    void addObserver(std::weak_ptr<Observer> obs) { m_observers.push_back(obs); }
    void removeObserver(std::weak_ptr<Observer> obs) {
        std::remove_if(m_observers.begin(), m_observers.end(), [&obs](const std::weak_ptr<Observer>& observer)
        {return obs.lock() == observer.lock();});
    }

private:
    KernelRelease m_kernel_release = {0};
    std::vector<std::weak_ptr<Observer>> m_observers;
    
    void updateObservers(UpdateMessage data){
        for(auto& observer: m_observers){
            if(auto obs = observer.lock()){
                obs->update(data);
            }
        }
    }
    std::vector<std::weak_ptr<Observer>> getObservers() { return m_observers; }
    void updateObserver(std::weak_ptr<Observer>& obs, UpdateMessage& data){
        for(auto& observer: m_observers){
            auto _obs = observer.lock();
            if(_obs == obs.lock()){
                _obs->update(data);
            }
        }
    }

    void getKernelRelease() {
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

    std::unique_ptr<Payload> makePayload(Command type, KernelRelease& ker, std::vector<std::string>& params);
};
#endif //TRGEN_CORE_HPP