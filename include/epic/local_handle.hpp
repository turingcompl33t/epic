// local_handle.hpp

#ifndef EPIC_LOCAL_HANDLE_H
#define EPIC_LOCAL_HANDLE_H

#include "local.hpp"
#include "collector.hpp"

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
}

#endif // EPIC_LOCAL_HANDLE_H