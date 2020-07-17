// pointer.hpp

#ifndef EPIC_POINTER_H
#define EPIC_POINTER_H

#include <memory>
#include <cstddef>

namespace epic
{
    template<typename T>
    class pointable
    {
    public:
        using init_t = T;

        // pointable::alignment()
        // Returns the alignment requirement of the pointed-to type.
        static inline auto alignment() -> size_t
        {
            return alignof(T);
        }

        // pointable::init()
        // Initializes a new pointable via perfect forwarding.
        template<typename... Args>
        static auto init(Args&&... args) -> size_t
        {
            auto box = std::make_unique<T>(std::forward<Args>(args)...);
            return reinterpret_cast<size_t>(box.release());
        }

        // pointable::deref()
        // Returns a reference to the pointed-to value.
        static auto deref(size_t ptr) -> T const&
        {
            auto p = reinterpret_cast<T*>(ptr);
            return *p;
        }

        // pointable::deref_mut()
        // Returns a reference to the pointed-to value.
        //
        // Equivalent to pointable::deref()
        static auto deref_mut(size_t ptr) -> T&
        {
            auto p = reinterpret_cast<T*>(ptr);
            return *p;
        }

        // pointable::drop()
        // Frees the memory owned by the pointed to value.
        static auto drop(size_t ptr) -> void
        {
            auto p = reinterpret_cast<T*>(ptr);
            delete p;
        }
    };
}

#endif // EPIC_POINTER_H 