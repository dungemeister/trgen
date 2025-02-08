#include "controller.hpp"
#include <iostream>

void Controller::loop() {
    while(true) {
        Task task;
        task.msg = m_cli.handle_command();
        
        m_cmd_queue.push(m_cur_task);
        m_cur_task = m_cmd_queue.front();

        run_task(m_cur_task);
        m_cmd_queue.pop();
    }
}

void Controller::help() {
    std::cout << "Usage:\n";
    Pinger::help();
}

void Controller::show_tasks() {
    if(m_cmd_queue.size() == 0) {
        std::cout << "No tasks in list\n";
        return;
    }

    
}

void Controller::run_task(Task& task) {
    std::string cmd = task.msg.command;
    std::cout << cmd << "\n";
    if(cmd == HELP_CMD) {
        help();
    }
    else {
        std::cout << "Unknown command\n";
    }
    task.state = FINISHED;

}