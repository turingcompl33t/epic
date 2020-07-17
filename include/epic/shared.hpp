// shared.hpp

#ifndef EPIC_SHARED_H
#define EPIC_SHARED_H

#include <cstddef>

#include "base.hpp"

namespace epic
{
    template <typename T>
    class owned;

    template <typename T>
    class shared
    {
        size_t data;

    public:
        shared() = delete;

        // NOTE: the shared instance does not own the pointee, and
        // thus does not attempt to destroy it on drop
        ~shared() = default;

        shared(shared const& s) 
            : data{s.data} {}
        
        shared& operator=(shared const& rhs)
        {
            this->data = rhs.data;
            return *this;
        }

        shared(shared&&)            = delete;
        shared& operator=(shared&&) = delete;

        // shared::null()
        // Returns a new null shared instance.
        static auto null() -> shared<T>
        {
            return shared<T>{0};
        }

        static auto from_raw(T const* raw) -> shared<T>
        {
            auto s = reinterpret_cast<size_t>(raw);
            ensure_aligned<T>(s);
            return shared<T>::from_usize(s);
        }

        // shared::with_tag()
        // Returns the same pointer, but tagged with `tag`.
        // The `tag` is truncated to fit into the unused
        // bits of the pointer to `T`.
        auto with_tag(size_t tag) -> shared<T>
        {
            return shared<T>::from_usize(compose_tag<T>(this->data, tag));
        }

        // shared::as_raw()
        // Converts the shared pointer to a raw pointer, without the tag.
        auto as_raw() -> T const*
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return reinterpret_cast<T const*>(r);
        }

        // shared::into_owned()
        // Takes ownership of the pointee.
        auto into_owned() -> owned<T>
        {
            assert(!this->is_null());
            return owned<T>::from_usize(this->data);
        }

        // shared::clone()
        auto clone() -> shared<T>
        {
            return shared<T>{this->data};
        }

        // shared::into_usize()
        auto into_usize() -> size_t
        {
            return this->data;
        }

        // shared::from_usize()
        static auto from_usize(size_t data) -> shared<T>
        {
            return shared<T>{data};
        }

        // shared::tag()
        // Returns the tag stored within the poiner.
        auto tag() const -> size_t
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return t;
        }

        // shared::is_null()
        // Returns `true` if the pointer is null.
        auto is_null() const -> bool
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return 0 == r;
        }

        auto operator*() -> T&
        {
            return this->deref_mut();
        }

        auto operator->() -> T*
        {
            return &this->deref_mut();
        }

    private:
        shared(size_t init) : data{init} {}

        // shared::deref()
        // Returns an immutable reference to the pointee.
        auto deref() -> T const&
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return pointable<T>::deref(r);
        }

        // shared::deref_mut()
        // Returns a mutable reference to the pointee.
        auto deref_mut() -> T&
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return pointable<T>::deref_mut(r);
        }
    };
};

#endif // EPIC_SHARED_H