#include "core.hpp"
#include <iostream>
#include "payload.hpp"
#include "pinger.hpp"
#include <memory>

void Core::run(Task& task) {

    std::string cmd_str = task.msg.command;
    std::unique_ptr<Payload> payload = nullptr;

    if(COMMAND_MAP.find(cmd_str) != COMMAND_MAP.end()) {
        Command cmd_int = COMMAND_MAP.at(cmd_str);
        switch(cmd_int) {
            case Command::ping:
                payload = std::make_unique<Pinger>(Pinger(m_kernel_release, task.msg.params));
            case Command::traceroute:
                break;
            
            default:
                std::cout << cmd_str << " is not implemented yet\n";
        }
        if(payload != nullptr)
            payload->payload_run();
    }
    else {
        std::cout << "Unknown command\n";
    }
    task.state = FINISHED;
}

