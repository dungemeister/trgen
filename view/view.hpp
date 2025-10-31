#ifndef TRGEN_VIEW_HPP
#define TRGEN_VIEW_HPP
#include "trgenTypes.hpp"

class View{
public:
    ~View() = default;
    virtual void update(UpdateMessage msg) = 0;
};
#endif