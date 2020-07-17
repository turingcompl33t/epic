// atomic.hpp

#ifndef EPIC_ATOMIC_H
#define EPIC_ATOMIC_H

#include <atomic>
#include <optional>

#include "base.hpp"
#include "owned.hpp"
#include "shared.hpp"
#include "pointer.hpp"
#include "ordering.hpp"

namespace epic
{
    class guard;

    template <typename T>
    using optional_shared = std::optional<shared<T>>;

    // An atomic pointer that can be safely shared between threads.
    //
    // The pointer must be properly aligned. Since it is aligned, a tag can
    // be stored in the least significant bits of the addres. For example,
    // the tag for a pointer to a sized type T should be less than:
    // 
    // Any method that loads the pointer must be passed a reference to an epic::guard.
    template <typename T>
    class atomic
    {
        std::atomic_size_t data;

    public:
        atomic()  = delete;
        ~atomic() = default;

        atomic(atomic const&)            = delete;
        atomic& operator=(atomic const&) = delete;

        atomic(atomic&&)            = delete;
        atomic& operator=(atomic&&) = delete;

        // atomic::make()
        // Constructs a new pointee on the heap and returns a new atomic pointer to it.
        template <typename... Args>
        static auto make(Args&&... args) -> atomic<T>
        {
            auto const r = pointable<T>::init(std::forward<Args>(args)...);
            return atomic<T>::from_usize(r);
        }

        // atomic::null()
        // Returns a new null atomic pointer.
        static auto null() -> atomic<T>
        {
            return atomic<T>{0};
        }

        // atomic::from_usize()
        // Returns a new atomic pointer pointing to tagged pointer `data`.
        static auto from_usize(size_t data) -> atomic<T>
        {
            return atomic<T>{data};
        }

        // atomic::from_raw()
        // Constructs a new `atomic` instance from a raw pointer.
        static auto from_raw(T const* ptr) -> atomic<T>
        {
            auto as_usize = reinterpret_cast<size_t>(ptr);
            return atomic<T>::from_usize(as_usize);
        }

        // atomic::from_unique()
        // Constructs a new `atomic` instance from a std::unique_ptr
        // and consumes the std::unique_ptr instance. 
        static auto from_unique(std::unique_ptr<T>&& ptr) -> atomic<T>
        {
            auto raw_ptr = ptr.release();
            return atomic<T>::from_raw(raw_ptr);
        }

        // atomic::from_shared()
        // Constructs a new `atomic` instance from a `shared` instance.
        static auto from_shared(shared<T>&& s) -> atomic<T>
        {
            return atomic<T>::from_usize(s.into_usize());
        }

        // atomic::from_owned()
        // Constructs a new `atomic` instance from an `owned` instance. 
        static auto from_owned(owned<T>&& o) -> atomic<T>
        {
            return atomic<T>::from_usize(o.into_usize());
        }

        // atomic::into_owned()
        // Takes ownership of the pointee.
        //
        // This operation consumes the `atomic` and converts it into an `owned` instance.
        // As an `atomic` doesn't have a destructor and therefore does not drop the pointee
        // as `owned` does, this is suitable for destructors of data structures.
        auto into_owned() -> owned<T>
        {   
            // NOTE: a relaxed load is performed here because this
            // operation consumes the atomic itself.
            auto const raw = std::atomic_load_explicit(&this->data, std::memory_order_relaxed);
            return owned<T>::from_usize(raw);
        }

        // atomic::clone()
        // Returns a copy of the atomic value. 
        auto clone() -> atomic<T>
        {
            auto const raw = std::atomic_load_explicit(&this->data, std::memory_order_relaxed);
            return atomic<T>::from_usize(raw);
        }

        // atomic::load()
        // Loads a `shared` from the atomic pointer.
        //
        // This function takes an ordering argument that describes
        // the memory ordering for the load operation.
        auto load(std::memory_order order, guard& g) -> shared<T>
        {   
            auto const l = std::atomic_load_explicit(&this->data, order);
            return shared<T>::from_usize(l);
        }

        // atomic::store(shared<T>)
        // Store the pointer managed by `shared` instance into `atomic`.
        //
        // Consumes the `shared` instance.
        auto store(shared<T>&& new_ptr, std::memory_order order) -> void
        {
            auto const raw = new_ptr.into_usize();
            std::atomic_store_explicit(&this->data, raw, order);
        }

        // atomic::store(owned<T>)
        // Store the pointer managed by `owned` instance into `atomic`.
        //
        // Consumes the `owned` instance.
        auto store(owned<T>&& new_ptr, std::memory_order order) -> void
        {
            auto const raw = new_ptr.into_usize();
            std::atomic_store_explicit(&this->data, raw, order);
        }

        // atomic::swap(shared<T>)
        // Stores a `shared` pointer into the atomic pointer, returning the 
        // previous pointer as a `shared`.
        auto swap(shared<T> new_ptr, std::memory_order order, guard& g) -> shared<T>
        {
            auto const prev = std::atomic_exchange_explicit(&this->data, new_ptr.into_usize(), order);
            return shared<T>::from_usize(prev);
        }

        // atomic::swap(owned<T>)
        // Stores an `owned` pointer into the atomic pointer, returning the
        // previous pointer as a `shared`.
        auto swap(owned<T> new_ptr, std::memory_order order, guard& g) -> shared<T>
        {
            auto const prev = std::atomic_exchange_explicit(&this->data, new_ptr.into_usize(), order);
            return shared<T>::from_usize(prev);
        }

        // atomic::compare_and_set(shared<T>)
        // Stores the pointer `next` into the atomic pointer if the current
        // value of the pointer is the same as `current`. The tag of the pointer
        // is also taken into account, so two pointers to the same object but
        // with distinct tags will not be considered equal.
        auto compare_and_set(
            shared<T> current, 
            shared<T> next, 
            std::memory_order order, 
            guard& g) -> optional_shared<T>
        {   
            auto const curr_raw = current.into_usize();
            auto const next_raw = next.into_usize();    
            auto const exchanged = std::atomic_compare_exchange_strong_explicit(
                &this->data, 
                curr_raw, 
                next_raw, 
                ordering_success(order), 
                ordering_failure(order));

            if (exchanged)
            {
                return std::make_optional<shared<T>>(shared<T>::from_usize(next_raw));
            }

            // failed to perform the exchange
            return std::nullopt;
        }

        // atomic::compare_and_set(owned<T>)
        // Stores the pointer `next` into the atomic pointer if the current
        // value of the pointer is the same as `current`. The tag of the pointer
        // is also taken into account, so two pointers to the same object but
        // with distinct tags will not be considered equal.
        auto compare_and_set(
            shared<T> current, 
            owned<T> next, 
            std::memory_order order, 
            guard& g) -> optional_shared<T>
        {   
            auto const curr_raw = current.into_usize();
            auto const next_raw = next.into_usize();    
            auto const exchanged = std::atomic_compare_exchange_strong_explicit(
                &this->data, 
                curr_raw, 
                next_raw, 
                ordering_success(order), 
                ordering_failure(order));

            if (exchanged)
            {
                return std::make_optional<owned<T>>(owned<T>::from_usize(next_raw));
            }

            // failed to perform the exchange
            return std::nullopt;
        }

        // atomic::compare_and_set_weak(shared<T>)
        // Stores the pointer `next` into the atomic pointer if the current
        // value of the pointer is the same as `current`. The tag of the pointer
        // is also taken into account, so two pointers to the same object but
        // with distinct tags will not be considered equal.
        //
        // Unlike atomic::compare_and_set(), this function is allowed to spuriously fail.
        auto compare_and_set_weak(
            shared<T> current, 
            shared<T> next, 
            std::memory_order order, 
            guard& g) -> optional_shared<T>
        {   
            auto const curr_raw = current.into_usize();
            auto const next_raw = next.into_usize();    
            auto const exchanged = std::atomic_compare_exchange_weak_explicit(
                &this->data, 
                curr_raw, 
                next_raw, 
                ordering_success(order), 
                ordering_failure(order));

            if (exchanged)
            {
                return std::make_optional<shared<T>>(shared<T>::from_usize(next_raw));
            }

            // failed to perform the exchange
            return std::nullopt;
        }

        // atomic::compare_and_set_weak(owned<T>)
        // Stores the pointer `next` into the atomic pointer if the current
        // value of the pointer is the same as `current`. The tag of the pointer
        // is also taken into account, so two pointers to the same object but
        // with distinct tags will not be considered equal.
        //
        // Unlike atomic::compare_and_set(), this function is allowed to spuriously fail.
        auto compare_and_set_weak(
            shared<T> current, 
            owned<T> next, 
            std::memory_order order, 
            guard& g) -> optional_shared<T>
        {   
            auto const curr_raw = current.into_usize();
            auto const next_raw = next.into_usize();    
            auto const exchanged = std::atomic_compare_exchange_weak_explicit(
                &this->data, 
                curr_raw, 
                next_raw, 
                ordering_success(order), 
                ordering_failure(order));

            if (exchanged)
            {
                return std::make_optional<owned<T>>(owned<T>::from_usize(next_raw));
            }

            // failed to perform the exchange
            return std::nullopt;
        }

        // atomic::fetch_and()
        // Performs a bitwise "and" operation on the current tag and the argument `value`
        // and sets the new tag to the result. Returns the previous pointer as `shared`.
        auto fetch_and(size_t value, std::memory_order order, guard& g) -> shared<T>
        {
            auto const res = (value | ~low_bits<T>());
            auto const prev = std::atomic_fetch_and_explicit(&this->data, res, order);
            return shared<T>::from_usize(prev);
        }

        // atomic::fetch_or()
        // Performs bitwise "or" operation on the current tag and the argument `value`
        // and sets the new tag to the result. Returns the previous pointer as `shared`.
        auto fetch_or(size_t value, std::memory_order order, guard& g) -> shared<T>
        {
            auto const res = (value & low_bits<T>());
            auto const prev = std::atomic_fetch_or_explicit(&this->data, res, order);
            return shared<T>::from_usize(prev);
        }

        // atomic::fetch_xor()
        // Performs bitwise "xor" operaton on the current tag and the argument `value`
        // and sets the new tag to the result. Returns the previous pointer as `shared`.
        auto fetch_xor(size_t value, std::memory_order order, guard& g) -> shared<T>
        {
            auto const res = (value & low_bits<T>());
            auto const prev = std::atomic_fetch_xor_explicit(&this->data, res, order);
            return shared<T>::from_usize(prev);
        }

        // atomic::is_null()
        // Returns `true` if the pointer is null.
        auto is_null() const -> bool
        {
            auto const [r, t] = decompose_tag<T>(this->data);
            return 0 == r;
        }

    private:
        atomic(size_t init) : data{init} {}
    };

    // epic::make_atomic()
    template <typename T, typename... Args>
    auto make_atomic(Args&&... args) -> atomic<T>
    {
        return atomic<T>::make(std::forward<Args>(args)...);
    }
}

#endif // EPIC_ATOMIC_H 