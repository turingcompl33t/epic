// global.hpp

#ifndef EPIC_GLOBAL_H
#define EPIC_GLOBAL_H

#include "bag.hpp"
#include "epoch.hpp"

#include <lowlock/list.hpp>
#include <lowlock/queue.hpp>

namespace epic
{
    class local;

    // epic::global
    //
    // The global data for a collector instance.
    struct global
    {
        // Number of bags to destroy.
        static usize_t const COLLECT_STEPS = 8;

        // The intrusive linked list of `local`s.
        lowlock::list locals;

        // The global queue of deferred functions.
        lowlock::queue<bag> deferred_functions;

        // The global epoch.
        atomic_epoch global_epoch;

        global();

        // global::push_bag()
        // Push the bag of deferred functions onto the global queue.
        auto push_bag(std::unique_ptr<bag>&& b) -> void;

        // global::collect()
        // Collects several bags from the global queue of deferred functions
        // and executes the deferred functions within.
        auto collect() -> void;

        // global::try_advance()
        // Attempts to advance the global epoch.
        //
        // The epoch only advances if all currently pinned participants
        // have been pinned in the current epoch.
        auto try_advance() -> epoch;
    };
}

#endif // EPIC_GLOBAL_H