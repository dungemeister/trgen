#ifndef TRGEN_CONTROLLER_HPP

#define TRGEN_CONTROLLER_HPP
#include "cli.hpp"
#include "core.hpp"
#include <queue>

const std::string HELP_CMD = "help";
enum task_state{
    
    RUNNING = 0,
    FINISHED = 1,
    CANCELED = 2,
};

struct Task{
    message msg;
    task_state state;
    Task(): state(RUNNING) {}
};

class Controller{
public:
    Controller(MainCli& cli, Core& core): m_cli(cli), m_core(core), m_cur_task(), m_cmd_queue() {}
    void loop();
    void help();
    void show_tasks();
    void run_task(Task& t);
private:
    MainCli& m_cli;
    Core& m_core;
private:
    Task m_cur_task;
    std::queue<Task> m_cmd_queue;
};
#endif //TRGEN_CONTROLLER_HPP