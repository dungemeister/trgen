#include "controller.hpp"
#include <iostream>
#include <memory>


void Controller::loop() {
    
    m_core->addObserver(std::weak_ptr<Observer>(m_cli));

    Task pingTask = {};
    pingTask.msg.command = "ping";
    pingTask.msg.params = {"google.com"};
    
    // m_core->run(pingTask);

    while(true) {
        Task task;
        task.msg = handle_command();
        
        m_cmd_queue.push(task);
        m_cur_task = m_cmd_queue.front();

        m_core->run(m_cur_task);
        m_cmd_queue.pop();
    }
}

message Controller::handle_command() {
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
    return msg;
}

void Controller::show_tasks() {
    if(m_cmd_queue.size() == 0) {
        std::cout << "No tasks in list\n";
        return;
    }
    
}
