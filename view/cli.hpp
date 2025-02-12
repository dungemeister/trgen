#ifndef TRGEN_CLI_HPP
#define TRGEN_CLI_HPP

#include <string>
#include <vector>
#include "trgen_types.hpp"

class MainCli{
public:
    MainCli() {}
    ~MainCli() {}

    message handle_command();
private:

};

#endif //TRGEN_CLI_HPP