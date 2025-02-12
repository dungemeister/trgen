#ifndef TRGEN_PAYLOAD_HPP
#define TRGEN_PAYLOAD_HPP

class Payload {
public:
    virtual ~Payload() {} ;
    virtual void payload_run() = 0;
    virtual void description() = 0;
    virtual void help() = 0;

};
#endif //TRGEN_PAYLOAD_HPP