// owned.hpp

#ifndef EPIC_OWNED_H
#define EPIC_OWNED_H

#include <memory>
#include <cstddef>
#include <cassert>

#include "base.hpp"
#include "guard.hpp"

namespace epic
{
    template <typename T>
    class shared;

    // An owned, heap-allocated object.
    //
    // Analogous to a std::unique_ptr<T>.
    //
    // The pointer must be properly aligned. Since it is aligned,
    // a tag can be stored in the unusued least significant bits of the address.
    template<typename T>
    class owned
    {
        size_t data;

    public:
        owned() = delete;

        ~owned()
        {
            // don't attempt to decompose and deref a nullptr
            if (0 != data)
            {
                auto const [r, t] = decompose_tag<T>(this->data);
                pointable<T>::drop(r);
            }
        }

        // The type owned<T> is non-copyable; in order to make a 
        // deep copy of an owned<T>, use owned::clone()
        owned(owned const&)            = delete;
        owned& operator=(owned const&) = delete;

        owned(owned&& o) 
            : data{o.data} 
        {
            o.data = 0;
        }

        owned& operator=(owned&& o)
        {
            if (&o != this)
            {
                data   = o.data;
                o.data = 0;
            }

            return *this;
        }

        // owned::make()
        // Construct the specified type on the heap and returns a new owned pointer to it.
        template <typename... Args>
        static auto make(Args&&... args) -> owned<T>
        {
            return owned::from_usize(pointable<T>::init(std::forward<Args>(args)...));
        }

        // owned::from_raw()
        // Returns a new owned pointer pointing to `raw`.
        static auto from_raw(T* raw) -> owned<T>
        {
            auto const r = reinterpret_cast<size_t>(raw);
            ensure_aligned<T>(r);
            return owned::from_usize(r);
        }

        // owned::into_unique()
        // Converts the owned pointer into a std::unique_ptr.
        //
        // IMPT: this operation consumes the owned<T> instance.
        auto into_unique() -> std::unique_ptr<T>
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return std::unique_ptr<T>{reinterpret_cast<T*>(r)};
        }

        // owned::from_unique()
        static auto from_unique(std::unique_ptr<T>&& ptr) -> owned<T>
        {
            return owned::from_raw(ptr.release());
        }

        // owned::into_shared()
        // Converts the pointer into an epic::shared.
        //
        // This operation consumes the owned<T> instance
        // because, by definition, the caller is relinquishing
        // exclusive ownership of the pointee. 
        static auto into_shared(owned<T>&& o, guard& g) -> shared<T>
        {
            const auto as_usize = owned<T>::into_usize(std::move(o));
            return shared<T>::from_usize(as_usize);
        }

        // owned::into_usize()
        // Converts and returns owned pointer as usize (size_t).
        //
        // This operation consumes the owned<T> instance
        // because, by definition, the caller is relinquishing
        // exclusive ownership of the pointee.
        static auto into_usize(owned<T>&& o) -> size_t
        {
            return o.data;
        }

        // owned::from_usize()
        // Construct a new owned<T> from given usize (size_t).
        static inline auto from_usize(size_t data) -> owned<T>
        {
            assert(0 != data);
            return owned<T>{data};
        }

        // owned::tag()
        // Returns the tag from the owned pointer.
        auto tag() const noexcept -> size_t
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return t;
        }

        // owned::with_tag()
        // Returns the same pointer, but tagged with `tag`.
        // `tag is truncated to fit into the unused bits of pointer to `T`.
        auto with_tag(size_t tag) -> owned<T>
        {
            auto data = this->into_usize();
            return owned<T>::from_usize(compose_tag<T>(data, tag));
        }

        // owned::clone()
        // Clones the owned instance.
        //
        // IMPT: value pointed to by owned pointer must implmenet copy constructor.
        //
        // TODO: constrain via concept? std::enable_if?
        auto clone() -> owned<T>
        {
            auto& tmp = this->deref_mut();
            return owned<T>::make(tmp);
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
        owned(size_t init) : data{init} {}

        auto deref() -> T const&
        {   
            auto const [r, t] = decompose_tag<T>(this->data);
            return pointable<T>::deref(r);
        }

        auto deref_mut() -> T&
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return pointable<T>::deref_mut(r);
        }
    };

    // epic::make_owned()
    // Factory function.
    template <typename T, typename... Args>
    auto make_owned(Args&&... args) -> owned<T>
    {
        return owned<T>::make(std::forward<Args>(args)...);
    }
}

#endif