// local_handle.cpp

#include <epic/local_handle.hpp>
#include <epic/local.hpp>

namespace epic
{
    local_handle::local_handle(local* local_ptr_) 
        : local_ptr{local_ptr_} {}

    local_handle::~local_handle()
    {
        local_ptr->release_handle();
    }

    auto local_handle::pin() const -> guard
    {
        return local_ptr->pin();
    }

    auto local_handle::is_pinned() const -> bool
    {
        return local_ptr->is_pinned();
    }

    auto local_handle::get_collector() const -> collector const&
    {
        return local_ptr->get_collector();
    }
}