#ifndef TRGEN_HELPER_HPP
#define TRGEN_HELPER_HPP

#include "trgenTypes.hpp"
#include <payload.hpp>

class Help: public Payload {
public:
    Help() {}
    Help(KernelRelease& ker, std::vector<std::string>& params) {}
    ~Help() {}
    void payloadRun() override;
    void help() override {};
private:

};
#endif //TRGEN_HELPER_HPP