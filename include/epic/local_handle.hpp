// local_handle.hpp

#ifndef EPIC_LOCAL_HANDLE_H
#define EPIC_LOCAL_HANDLE_H

#include "guard.hpp"
#include "collector.hpp"

namespace epic
{
    class local;

    // local_handle
    //
    // A handle to a garbage collector instance.
    class local_handle
    {
        local* local_ptr;

    public:
        local_handle(local* local_ptr_);
        
        // The destructor for a `local_handle` releases a
        // handle to the associated `local` instance in the 
        // global list of `local`s maintained by the collector.
        ~local_handle();

        // local_handle::pin()
        auto pin() const -> guard;

        // local_handle::is_pinned()
        auto is_pinned() const -> bool;

        // local_handle::collector()
        auto get_collector() const -> collector const&;
    };
}

#endif // EPIC_LOCAL_HANDLE_H