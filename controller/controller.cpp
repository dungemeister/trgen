#include "controller.hpp"
#include <iostream>

void Controller::loop() {
    
    Task defaulTask;
    defaulTask.msg.command = "ping";
    defaulTask.msg.params = {"google.com"};
    // run_task(defaulTask);

    while(true) {
        Task task;
        task.msg = m_cli.handle_command();
        
        m_cmd_queue.push(task);
        m_cur_task = m_cmd_queue.front();

        run_task(m_cur_task);
        m_cmd_queue.pop();
    }
}

void Controller::help() {
    std::cout << "Usage:\n";
    Core::help();
}

void Controller::show_tasks() {
    if(m_cmd_queue.size() == 0) {
        std::cout << "No tasks in list\n";
        return;
    }
    
}

void Controller::run_task(Task& task) {
    std::string cmd_str = task.msg.command;
    if(COMMAND_MAP.find(cmd_str) != COMMAND_MAP.end()) {
        Command cmd_int = COMMAND_MAP.at(cmd_str);
        switch(cmd_int) {
            case Command::ping:
                m_core.ping(task.msg.params);
                break;
            case Command::help:
                help();
                break;
            default:
                std::cout << cmd_str << " is not implemented yet\n";
        }
    }
    else {
        std::cout << "Unknown command\n";
    }

    task.state = FINISHED;

}