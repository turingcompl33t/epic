// owned.hpp

#ifndef EPIC_OWNED_H
#define EPIC_OWNED_H

#include <memory>
#include <cstddef>
#include <cassert>

#include "base.hpp"
#include "shared.hpp"

namespace epic
{
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

        // impl drop for owned
        ~owned()
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            pointable<T>::drop(r);
        }

        owned(owned const&)            = delete;
        owned& operator=(owned const&) = delete;

        owned(owned&&)            = delete;
        owned& operator=(owned&&) = delete;

        // owned::create()
        // Construct the specified type on the heap and returns a new owned pointer to it.
        template <typename... Args>
        static auto create(Args&&... args) -> owned<T>
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
            auto const [raw, tag] = decompose_tag<T>(this->data);
            return std::unique_ptr<T>{reinterpret_cast<T*>(raw)};
        }

        // owned::from_unique()
        static auto from_unique(std::unique_ptr<T>&& ptr) -> owned<T>
        {
            return owned::from_raw(ptr.release());
        }

        // owned::into_shared()
        // Converts the pointer into an epic::shared.
        //
        // IMPT: this operation consumes the owned<T> instance.
        auto into_shared() -> shared<T>
        {
            return shared<T>::from_usize(this->into_usize());
        }

        // owned::into_usize()
        // Converts and returns owned pointer as usize (size_t).
        //
        // IMPT: this operation consumes the owned<T> instance.
        //
        // Trait: owned<T> implements Pointer<T>
        auto into_usize() -> size_t
        {
            auto const d = this->data;
            return d;
        }

        // owned::from_usize()
        // Construct a new owned<T> from given usize (size_t).
        //
        // Trait: owned<T> implements Pointer<T>
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
        // IMPT: value pointed to by owned pointer must implement clone().
        // auto clone() -> owned<T>
        // {

        // }

        // owned::deref
        // 
        // Trait:: owned<T> implements Deref<T>
        auto deref() -> T&
        {   
            auto const [r, t] = decompose_tag<T>(this->data);
            return pointable<T>::deref(r);
        }

        // owned::deref_mut()
        // 
        // Trait: owned<T> implements DerefMut<T>
        auto deref_mut() -> T&
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return pointable<T>::deref_mut(r);
        }

        // owned::as_ref()
        // Return a reference to the owned value.
        //
        // Trait: owned<T> implements AsRef<T>
        auto as_ref() -> T&
        {
            return this->deref();
        }

        // owned::as_mut()
        // Return a reference to the owned value.
        //
        // Trait: owned<T> implements AsMut<T>
        auto as_mut() -> T&
        {
            return this->deref_mut();
        }

    private:
        owned(size_t init) : data{init} {}
    };
}

#endif