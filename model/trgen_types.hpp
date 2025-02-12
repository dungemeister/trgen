#ifndef TRGEN_TYPES_HPP

#define TRGEN_TYPES_HPP
#include <unordered_map>

struct message{
    std::string command;
    std::vector<std::string> params;
};

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

enum class Command {
    help = 0,
    ping,
    traceroute,
    quit,

};

const std::unordered_map<std::string, Command> COMMAND_MAP = {
    {"help", Command::help},
    {"ping", Command::ping},
    {"traceroute", Command::traceroute},
    {"quit", Command::quit},

};

struct kernel_release {
    int kernel;
    int major;
    int minor;
    int patch;

    bool operator>=(const kernel_release& other) const {
        if(kernel == other.kernel && major == other.major && other.minor == minor && other.patch >= patch) return true;
        if(kernel == other.kernel && major == other.major && other.minor >= minor) return true;
        if(kernel == other.kernel && major >= other.major) return true;
        if(kernel > other.kernel) return true;
        
        return false;
    }

    bool operator<=(const kernel_release& other) const {
        if(kernel == other.kernel && major == other.major && other.minor == minor && other.patch <= patch) return true;
        if(kernel == other.kernel && major == other.major && other.minor <= minor) return true;
        if(kernel == other.kernel && major <= other.major) return true;
        if(kernel < other.kernel) return true;
        
        return false;
    }
    bool operator==(const kernel_release& other) const {
        return (other.kernel == kernel && other.major == major &&
                other.minor == minor && other.patch == patch);
    }

};

enum class exit_types {
    NEEDED_ROOT_PRIVILAGES = 1,

};
#endif //TRGEN_TYPES_HPP