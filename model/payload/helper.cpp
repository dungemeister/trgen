#include "helper.hpp"
#include "iostream"

void Help::payloadRun() {
    std::cout << "List of utilities:\n";
    Pinger::description();
    IfacesList::description();
}