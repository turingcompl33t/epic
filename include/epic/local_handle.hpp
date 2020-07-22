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
        local const* local_ptr;

    public:
        local_handle(local* local_ptr_) 
            : local_ptr{local_ptr_} {}

        ~local_handle();

        // local_handle::pin()
        __always_inline auto pin() const -> guard;

        // local_handle::is_pinned()
        __always_inline auto is_pinned() const -> bool;

        // local_handle::collector()
        __always_inline auto get_collector() const -> collector&;
    };
}

#endif // EPIC_LOCAL_HANDLE_H