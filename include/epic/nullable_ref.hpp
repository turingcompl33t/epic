// nullable_ref.hpp

#ifndef EPIC_NULLABLE_REF_H
#define EPIC_NULLABLE_REF_H

#include <stdexcept>

namespace epic
{   
    // nullable_ref
    //
    // A reference type thatis either a value reference
    // to `T`, or null (not present).
    template <typename T>
    class nullable_ref
    {
        T* ptr;

    public:
        nullable_ref() : ptr{nullptr} {}
        nullable_ref(T* ptr_) : ptr{ptr_} {}

        nullable_ref(nullable_ref const& other) 
            : ptr{other.ptr} {}

        nullable_ref& operator=(nullable_ref const& rhs)
        {
            this->ptr = rhs.ptr;
            return *this;
        }

        nullable_ref(nullable_ref&& other) 
            : ptr{other.ptr} {}

        nullable_ref& operator=(nullable_ref&& rhs)
        {
            if (&rhs != this)
            {
                this->ptr = rhs.ptr;
            }

            return *this;
        }

        auto operator*() -> T&
        {
            if (nullptr == ptr)
            {
                throw std::runtime_error{"attempt to dereference invalid nullable_ref"};
            }

            return *(this->ptr);
        }

        explicit operator bool()
        {
            return ptr != nullptr;
        }
    };
}

#endif // EPIC_OPTIONAL_REF_H