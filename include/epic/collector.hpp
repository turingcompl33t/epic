// collector.hpp

#ifndef EPIC_COLLECTOR_H
#define EPIC_COLLECTOR_H

#include <memory>

namespace epic
{
    struct global;
    class local_handle;

    // epic::collector
    //
    // An epoch-based garbage collector instance.
    struct collector
    {
        // The shared global data.
        std::shared_ptr<global> instance;

        collector();

        collector(collector const& c);

        collector& operator=(collector const& c);

        collector(collector&& c);

        collector& operator=(collector&& c);

        // collector::register_handle()
        // Register a new handle with the collector.
        //
        // Once a new `collector` instance is initialized,
        // the `register_handle()` method is the way in which
        // a thread registers itself for participation in
        // garbage collection. Thus, in the non-default (thread-local)
        // API, this is the entry point for individual threads.
        auto register_handle() -> local_handle;

        // collector::release()
        // Release reference to the global shared state.
        auto release() -> void;
    };
}

#endif // EPIC_COLLECTOR_H