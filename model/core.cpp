#include "core.hpp"
#include <iostream>

void Core::ping(std::vector<std::string> params) {

    Pinger pinger = Pinger(m_kernel_release, params);

    pinger.ping();
}

