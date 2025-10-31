#ifndef TRGEN_OBSERVER_HPP
#define TRGEN_OBSERVER_HPP
#include <string>
#include "trgenTypes.hpp"

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const UpdateMessage msg) = 0;
};
#endif //TRGEN_OBSERVER_HPP