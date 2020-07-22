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
        lowlock::queue<bag*> deferred_functions;

        // The global epoch.
        atomic_epoch global_epoch;

        global() 
            : locals{}
            , deferred_functions{}
            , global_epoch{epoch{}}
        {}

        // global::push_bag()
        // Push the bag of deferred functions onto the global queue.
        auto push_bag(std::unique_ptr<bag>&& b) -> void
        {
            // TODO: atomic fence?

            // Seal the bag with the current global epoch.
            auto const e = global_epoch.load(std::memory_order_relaxed);
            b->seal(e);

            // Push the bag onto the global queue.
            deferred_functions.push(b.release());
        }

        // global::collect()
        // Collects several bags from the global queue of deferred functions
        // and executes the deferred functions within.
        auto collect() -> void
        {
            // Attempt to advance the global epoch. 
            auto e = try_advance();

            auto const steps = COLLECT_STEPS;
            for (auto i = 0; i < steps; ++i)
            {
                // Pop a bag from the queue, provided it is expired.
                auto* popped_bag = deferred_functions.try_pop_if(
                    [e](bag* b){ return b->is_expired(); });
                if (!popped_bag.has_value())
                {
                    // Queue is empty or the bag is not expired, done collecting for now.
                    break;
                }

                // Otherwise, we got a valid bag of deferred functions, execute them.
                delete popped_bag;
            }
        }

        // global::try_advance()
        // Attempts to advance the global epoch.
        //
        // The epoch only advances if all currently pinned participants
        // have been pinned in the current epoch.
        auto try_advance() -> epoch
        {
            auto ge = global_epoch.load(std::memory_order_relaxed);
            // TODO: atomic fence??

            auto const broken = lowlock::list::iterate_while(
                [](lowlock::list_entry* e){ ; },
                [=](lowlock::list_entry* e)
                {
                    // Get a reference to the local for the element.
                    auto& l = local::element_of(e);
                    // Query the current local epoch.
                    auto local_epoch = l.get_epoch();
                    // Determine if the local is pinned in a different epoch.
                    return local_epoch.is_pinned() 
                        && local_epoch.unpinned() != ge;
                });

            // TODO: atomic fence??

            // All pinned participants are pinned in the current global epoch;
            // therefore it is appropriate the advance the global epoch.
            //
            // Note that if another thread already advanced the global epoch
            // in front of us, this store will simply overwrite the global epoch
            // with the same value (a dead store). This is true because `try_advance()`
            // was called from a thread that was pinned in `global_epoch`, and 
            // the global epoch cannot be advanced two steps ahead of it.
            auto new_epoch = global_epoch.successor();
            global_epoch.store(new_epoch, std::memory_order_release);
        }
    };
}

#endif // EPIC_GLOBAL_H