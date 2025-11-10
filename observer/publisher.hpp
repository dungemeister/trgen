#ifndef TRGEN_PUBLISHER_HPP
#define TRGEN_PUBLISHER_HPP

#include "observer.hpp"
#include <vector>
#include <memory>
#include <stdarg.h>
#include <functional>
#include <algorithm>

class Publisher {
public:
    ~Publisher()=default;

    void addObserver(std::weak_ptr<Observer> obs) { m_observers.push_back(obs); }
    void removeObserver(std::weak_ptr<Observer> obs) {
        std::remove_if(m_observers.begin(), m_observers.end(), [&obs](const std::weak_ptr<Observer>& observer)
        {return obs.lock() == observer.lock();});
    }
    
    void updateObservers(UpdateMessage data){
        for(auto& observer: m_observers){
            if(auto obs = observer.lock()){
                obs->update(data);
            }
        }
    }
    
    void updateObserver(std::weak_ptr<Observer>& obs, UpdateMessage& data){
        for(auto& observer: m_observers){
            auto _obs = observer.lock();
            if(_obs == obs.lock()){
                _obs->update(data);
            }
        }
    }

    void sendFormatedStringToObservers(const char* fmt, ...) {
        std::string prefix = "";
        va_list args;
        va_start(args, fmt);

        va_list(argsCopy);
        va_copy(argsCopy, args);
        int size = vsnprintf(nullptr, 0, fmt, argsCopy);
        va_end(argsCopy);
        std::string result = prefix;
        result.resize(size + prefix.size());
        vsnprintf(&result[prefix.size()], size+1, fmt, args);
        va_end(args);
        updateObservers({UpdateMessageTypes::str, result});
    }
private:
    std::vector<std::weak_ptr<Observer>> m_observers;
};
#endif //TRGEN_PUBLISHER_HPP