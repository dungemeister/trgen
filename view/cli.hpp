#ifndef TRGEN_CLI_HPP
#define TRGEN_CLI_HPP

#include <string>
#include <vector>
#include <iostream>
#include "trgenTypes.hpp"
#include "observer.hpp"
#include "view.hpp"

class MainCli: public Observer, public View{
public:
    ~MainCli() {}
    void update(const UpdateMessage msg) {std::cout<<msg.data;};
private:

};

#endif //TRGEN_CLI_HPP