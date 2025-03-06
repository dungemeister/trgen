#ifndef TRGEN_PAYLOAD_HPP
#define TRGEN_PAYLOAD_HPP
#include <string>
#include <memory>
#include "subject.hpp"

class Payload: public Subject {
public:
    virtual ~Payload() {} ;
    virtual void payloadRun() = 0;
    virtual void help() = 0;
    // virtual std::string updateView(std::string str) = 0;


};


#endif //TRGEN_PAYLOAD_HPP