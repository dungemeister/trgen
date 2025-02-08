#ifndef TRGEN_CLI_HPP
#define TRGEN_CLI_HPP

#include <string>
#include <vector>

struct message{
    std::string command;
    std::vector<std::string> params;
};

class MainCli{
public:
    MainCli() {}
    ~MainCli() {}

    message handle_command();
private:

};

#endif //TRGEN_CLI_HPP