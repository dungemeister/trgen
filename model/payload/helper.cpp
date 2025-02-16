#include "helper.hpp"
#include "iostream"
#include "pinger.hpp"
#include "ifacesList.hpp"

void Help::payloadRun() {
    std::cout << "List of utilities:\n";
    Pinger::description();
    IfacesList::description();
}