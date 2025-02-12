#ifndef TRGEN_CONTROLLER_HPP

#define TRGEN_CONTROLLER_HPP
#include "cli.hpp"
#include "core.hpp"
#include <queue>
#include <unordered_set>
#include <unordered_map>


class Controller{
public:
    Controller(MainCli& cli, Core& core): m_cli(cli), m_core(core), m_cur_task(), m_cmd_queue() {}
    void loop();
    void show_tasks();
private:
    MainCli& m_cli;
    Core& m_core;
private:
    Task m_cur_task;
    std::queue<Task> m_cmd_queue;
};
#endif //TRGEN_CONTROLLER_HPP