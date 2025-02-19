#ifndef TRGEN_TYPES_HPP

#define TRGEN_TYPES_HPP
#include <unordered_map>
#include <string>
#include <vector>


struct UpdateMessage{
    std::string data;
};

struct message{
    std::string command;
    std::vector<std::string> params;
};

enum TaskState{
    
    RUNNING = 0,
    FINISHED = 1,
    CANCELED = 2,
};

struct Task{
    message msg;
    TaskState state;
    Task(): state(RUNNING) {}
};

enum class Command {
    help = 0,
    list,
    ping,
    traceroute,
    quit,

};

const std::unordered_map<std::string, Command> COMMAND_MAP = {
    {"help", Command::help},
    {"ping", Command::ping},
    {"iflist", Command::list},
    {"traceroute", Command::traceroute},
    {"quit", Command::quit},

};

struct KernelRelease {
    int kernel;
    int major;
    int minor;
    int patch;

    bool operator>=(const KernelRelease& other) const {
        if(kernel == other.kernel && major == other.major && other.minor == minor && other.patch >= patch) return true;
        if(kernel == other.kernel && major == other.major && other.minor >= minor) return true;
        if(kernel == other.kernel && major >= other.major) return true;
        if(kernel > other.kernel) return true;
        
        return false;
    }

    bool operator<=(const KernelRelease& other) const {
        if(kernel == other.kernel && major == other.major && other.minor == minor && other.patch <= patch) return true;
        if(kernel == other.kernel && major == other.major && other.minor <= minor) return true;
        if(kernel == other.kernel && major <= other.major) return true;
        if(kernel < other.kernel) return true;
        
        return false;
    }
    bool operator==(const KernelRelease& other) const {
        return (other.kernel == kernel && other.major == major &&
                other.minor == minor && other.patch == patch);
    }

};

enum class ExitTypes {
    NEEDED_ROOT_PRIVILAGES = 1,

};

struct IfaceInfo{
    std::string name;
    std::string macAddr;
    std::vector<std::pair<std::string, std::string>> ipAddr;
    std::string state;
    std::string nsName;
};
#endif //TRGEN_TYPES_HPP