// collector.hpp

#ifndef EPIC_COLLECTOR_H
#define EPIC_COLLECTOR_H

#include <memory>

#include "global.hpp"

namespace epic
{
    // collector
    //
    // An epoch-based garbage collector instance.
    class collector
    {
        // The shared global data.
        std::shared_ptr<global> instance;

    public:
        collector() 
            : instance{std::move(std::make_shared<global>())} {}

        collector(collector const& c) 
            : instance{c.instance} {}

        collector& operator=(collector const& c)
        {
            instance = c.instance;
            return *this;
        }

        collector(collector&& c) 
            : instance{std::move(c.instance)}
        {}

        collector& operator=(collector&& c)
        {
            if (&c != this)
            {
                instance = std::move(c.instance);
            }

            return *this;
        }
    };
}

#endif // EPIC_COLLECTOR_H