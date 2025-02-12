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

        m_core.run(m_cur_task);
        m_cmd_queue.pop();
    }
}

void Controller::show_tasks() {
    if(m_cmd_queue.size() == 0) {
        std::cout << "No tasks in list\n";
        return;
    }
    
}
