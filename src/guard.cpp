// guard.cpp

#include <epic/guard.hpp>
#include <epic/local.hpp>
#include <epic/scope_guard.hpp>

namespace epic
{
    guard::guard() : local_ptr{nullptr} {}

    guard::guard(local const* local_ptr_) 
        : local_ptr{local_ptr_} {}

    guard::~guard()
    {
        if (!is_dummy())
        {
            local_ptr->unpin();
        }
    }
    
    auto guard::defer(std::function<void()>&& f) -> void
    {
        if (is_dummy())
        {
            // immediately invoke the deferred function for dummy guards
            f();
        }
        else
        {
            // otherwise, add to the thread-local cache
            local_ptr->defer(std::move(deferred{std::move(f)}), *this);
        }
    }

    template <typename T>
    auto guard::defer_destroy(shared<T>&& ptr) const -> void
    {
        // calling shared::into_owned() on the given `shared` instance
        // returns an `owned<T>`, which, immediately after it is produced,
        // falls out of scope and is cleaned up by its destructor
        // (recall that `shared<T>` does not destroy pointee on destruction)
        defer([ptr = std::move(ptr)](){ ptr.into_owned(); });
    }

    auto guard::flush() -> void
    {
        if (!is_dummy())
        {
            local_ptr->flush(*this);
        }
    }

    auto guard::repin() -> void
    {
        if (!is_dummy())
        {
            local_ptr->repin();
        }
    }

    template <typename R>
    auto guard::repin_after(std::function<R()>&& f) -> R
    {
        if (!is_dummy())
        {
            local_ptr->acquire_handle();
            local_ptr->unpin();
        }

        // Require a scope guard here to repin and release handle
        // to handle the event in which the provided function throws.
        scope_guard sg{[=]()
        { 
            local_ptr->pin();
            local_ptr->release_handle();
        }};

        return f();
    } 

    auto guard::is_dummy() const noexcept -> bool
    {
        return nullptr == local_ptr;
    }

    auto guard::unprotected() -> guard
    {
        return guard{};
    }
}