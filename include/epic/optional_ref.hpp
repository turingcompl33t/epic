// optional_ref.hpp

#ifndef EPIC_OPTIONAL_REF_H
#define EPIC_OPTIONAL_REF_H

#include <stdexcept>

namespace epic
{   
    // optional_ref
    template <typename T>
    class optional_ref
    {
        T* ptr;

    public:
        optional_ref() : ptr{nullptr} {}
        optional_ref(T* ptr_) : ptr{ptr_} {}

        optional_ref(optional_ref const& other) 
            : ptr{other.ptr} {}

        optional_ref& operator=(optional_ref const& rhs)
        {
            this->ptr = rhs.ptr;
            return *this;
        }

        optional_ref(optional_ref&& other) 
            : ptr{other.ptr} {}

        optional_ref& operator=(optional_ref&& rhs)
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
                throw std::runtime_error{"attempt to dereference invalid optional_ref"};
            }

            return *(this->ptr);
        }

        explicit auto operator bool() -> bool
        {
            return ptr != nullptr;
        }
    };
}

#endif // EPIC_OPTIONAL_REF_H