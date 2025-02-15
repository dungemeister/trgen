#include "core.hpp"
#include <iostream>

#include <memory>

void Core::run(Task& task) {
    
    std::string cmd_str = task.msg.command;
    if(COMMAND_MAP.at(cmd_str) == Command::quit)
        exit(0);
    
    std::unique_ptr<Payload> payload = nullptr;

    if(COMMAND_MAP.find(cmd_str) != COMMAND_MAP.end()) {
        Command cmd_int = COMMAND_MAP.at(cmd_str);
        payload = makePayload(cmd_int, m_kernel_release, task.msg.params);
        if(payload != nullptr)
            payload->payloadRun();
    }
    else {
        std::cout << "Unknown command\n";
    }
    task.state = FINISHED;
}

std::unique_ptr<Payload> Core::makePayload(Command type, kernel_release& ker, std::vector<std::string>& params) {
    switch(type) {
        case Command::list:
            return std::make_unique<IfacesList>(params);
        case Command::ping:
            return std::make_unique<Pinger>(ker, params);
        case Command::help:
            return std::make_unique<Help>();
        default:
            return nullptr;
    };
}