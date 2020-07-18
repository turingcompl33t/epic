// collector.hpp

#ifndef EPIC_COLLECTOR_H
#define EPIC_COLLECTOR_H

#include "guard.hpp"
#include "internal.hpp"

namespace epic
{
    // local_handle
    //
    // A handle to a garbage collector instance.
    class local_handle
    {
        local const* local_ptr;

    public:
        ~local_handle()
        {
            local_ptr->release_handle();
        }

        // local_handle::pin()
        __always_inline auto pin() const -> guard
        {
            return local_ptr->pin();
        }

        // local_handle::is_pinned()
        __always_inline auto is_pinned() const -> bool
        {
            return local_ptr->is_pinned();
        }

        // local_handle::collector()
        __always_inline auto collector() const -> collector&
        {
            return local_ptr->collector();
        }
    };

    // collector
    //
    // An epoch-based garbage collector instance.
    class collector
    {
        // The shared global data.
        std::shared_ptr<global> instance;

    public:
        collector(collector const&)            = delete;
        collector& operator=(collector const&) = delete;

        collector(collector&& c) 
            : instance{std::move(c.instance)}
        {}

        collector& operator=(collector&& c)
        {
            if (&c != this)
            {
                this->instance = std::move(c.instance);
            }

            return *this;
        }

        // collector::make()
        // Create a new collector instance.
        static auto make() -> collector
        {
            return collector{};
        }

        // collector::global_as_ref()
        // Returns a reference to the global data for this instance.
        auto global_as_ref() -> global&
        {
            return *instance.get();
        }

    private:
        collector() 
            : instance{std::move(global::make())}
        {}
    };
}

#endif // EPIC_COLLECTOR_H