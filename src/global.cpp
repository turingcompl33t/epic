// global.cpp

#include <epic/global.hpp>
#include <epic/local.hpp>

#include <cassert>

namespace epic
{
    global::global() 
        : locals{}
        , deferred_functions{}
        , global_epoch{epoch{}}
    {}

    auto global::push_bag(std::unique_ptr<bag>&& b) -> void
    {
        // TODO: atomic fence?

        // Seal the bag with the current global epoch.
        auto const e = global_epoch.load(std::memory_order_relaxed);
        b->seal(e);

        // Push the bag onto the global queue.
        deferred_functions.push(b.release());
    }

    auto global::collect() -> void
    {
        // Attempt to advance the global epoch. 
        auto e = try_advance();

        auto const steps = global::COLLECT_STEPS;
        for (auto i = 0; i < steps; ++i)
        {
            // Pop a bag from the queue, provided it is expired.
            std::optional<bag*> popped_bag = deferred_functions.try_pop_if(
                [&e](bag* b) -> bool 
                { 
                    return b->is_expired(e); 
                });

            if (!popped_bag.has_value())
            {
                // Queue is empty or the bag is not expired, done collecting for now.
                break;
            }

            // Otherwise, we got a valid bag of deferred functions, execute them.
            delete popped_bag.value();
        }
    }

    auto global::try_advance() -> epoch
    {
        auto ge = global_epoch.load(std::memory_order_relaxed);
        // TODO: atomic fence??

        auto const broken = locals.iterate_while(
            [](lowlock::list_entry* e){ ; },
            [=](lowlock::list_entry* e) -> bool
            {
                // Get a reference to the local for the element.
                auto& l = local::element_of(*e);
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
        auto new_epoch = ge.successor();
        global_epoch.store(new_epoch, std::memory_order_release);

        return new_epoch;
    }
}