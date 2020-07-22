// local_handle.cpp

#include "local_handle.hpp"
#include "local.hpp"

namespace epic
{
    local_handle::~local_handle()
    {
        local_ptr->release_handle();
    }

    __always_inline auto local_handle::pin() const -> guard
    {
        return local_ptr->pin();
    }

    __always_inline auto local_handle::is_pinned() const -> bool
    {
        return local_ptr->is_pinned();
    }

    __always_inline auto local_handle::get_collector() const -> collector&
    {
        return local_ptr->get_collector();
    }
}