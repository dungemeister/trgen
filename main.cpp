#include <iostream>
#include <unistd.h>
#include <memory>
#include "controller.hpp"
#include "core.hpp"
#include "cli.hpp"

int main(int argc, char **argv) {
    std::cout << "Current PID - " << getpid() << "\n";
    auto cli = std::make_shared<MainCli>();
    auto core = std::make_shared<Core>();

    Controller ctrl = Controller(cli, core);
    ctrl.loop();
    
    return 0;
}
