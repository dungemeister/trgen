#include "cli.hpp"
#include <iostream>

message MainCli::handle_command() {
    std::cout << "Waiting for a command\n";
    message msg;
    std::string line;
    std::string delimiter = " ";

    getline(std::cin, line);

    if(line.find(delimiter) != std::string::npos) {
        msg.command = line.substr(0, line.find(delimiter));
        line.erase(0, line.find(delimiter) + delimiter.length());
        while(line.find(delimiter) != std::string::npos) {
            msg.params.push_back(line.substr(0, line.find(delimiter)));
            line.erase(0, line.find(delimiter) + delimiter.length());
        }
        msg.params.push_back(line.substr(0, line.find("\n")));
    }
    else {
        msg.command = line;
    }
    std::cout << "entered command is [" << msg.command << "]";
    for(auto param: msg.params) {
        std::cout << " " << param;
    }
    std::cout << "\n";
    return msg;
}

