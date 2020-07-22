// global.hpp

#ifndef EPIC_GLOBAL_H
#define EPIC_GLOBAL_H

#include <cstddef>
#include <optional>

#include "bag.hpp"
#include "epoch.hpp"

#include <lowlock/list.hpp>
#include <lowlock/queue.hpp>

namespace epic
{
    // global
    //
    // The global data for a collector instance.
    class global
    {
        // Number of bags to destroy.
        static usize_t const COLLECT_STEPS = 8;

        // The intrusive linked list of `local`s.
        lowlock::list locals;

        // The global queue of deferred functions.
        lowlock::queue<bag*> deferred_functions;

        // The global epoch.
        atomic_epoch global_epoch;

    public:
        global() 
            : locals{}
            , deferred_functions{}
            , global_epoch{epoch{}}
        {}
    };
}

#endif // EPIC_GLOBAL_H