#include <iostream>
#include <unistd.h>
#include "controller.hpp"
#include "core.hpp"
#include "cli.hpp"

int main(int argc, char **argv) {
    std::cout << "Current PID - " << getpid() << "\n";
    MainCli cli;
    Core core;

    Controller ctrl = Controller(cli, core);
    ctrl.loop();
    
    return 0;
}
