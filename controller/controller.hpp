#ifndef TRGEN_CONTROLLER_HPP

#define TRGEN_CONTROLLER_HPP
#include "cli.hpp"
#include "core.hpp"
#include <queue>
#include <unordered_set>
#include <unordered_map>


class Controller{
public:
    Controller(std::shared_ptr<MainCli> cli,
               std::shared_ptr<Core> core): m_cli(cli), m_core(core), m_cur_task(), m_cmd_queue() {}
    void loop();
    message handle_command();
    void show_tasks();

private:
    std::shared_ptr<MainCli> m_cli;
    std::shared_ptr<Core> m_core;
    Task m_cur_task;
    std::queue<Task> m_cmd_queue;
    std::vector<std::weak_ptr<Observer>> m_observers;


};
#endif //TRGEN_CONTROLLER_HPP