// epoch.hpp

#ifndef EPIC_EPOCH_H
#define EPIC_EPOCH_H

#include "ordering.hpp"
#include "type_alias.hpp"

namespace epic
{
    // The global epoch
    //
    // The last bit in the global epoch is unused and is always zero.
    // Every so often the global epoch is incremented i.e. we say it
    // "advances". A pinned participant may advance the flobal epoch
    // only if all currently pinned participants have been pinned in
    // the current epoch. 

    // An epoch that can be marked as pinned or unpinned.
    //
    // Internally, the epoch is represented as an integer that
    // wraps around at some unspecified point and a flag that represents
    // whether the epoch is pinned or unpinned.
    class epoch
    {
        usize_t data;

    public:

        epoch(epoch const&)            = delete;
        epoch& operator=(epoch const&) = delete;

        epoch(epoch&& e) : data{e.data} {}
        
        epoch& operator=(epoch&& e)
        {
            if (&e != this)
            {
                this->data = e.data;
                e.data = 0;
            }

            return *this;
        }

        // epoch::starting()
        // Returns the starting epoch in the unpinned state.
        static auto starting() -> epoch
        {
            return epoch{};
        }

        // epoch::with_value()
        // Returns a new epoch with the given value.
        static auto with_value(usize_t v) -> epoch
        {
            return epoch{v};
        }

        // epoch::wrapping_sub()
        // Returns the number of epochs `a` is ahead of `b`.
        //
        // Internally, epochs are represented as numbers in the range 
        // (ISIZE_MIN / 2) .. (ISIZE_MAX / 2), so the returned distance
        // will also be within this interval.
        static auto wrapping_sub(epoch&& a, epoch&& b) -> isize_t
        {
            auto const masked = (b.data & ~1ul);
            return static_cast<isize_t>(a.data - masked) >> 1;
        }

        // epoch::is_pinned()
        // Return `true` if the epoch is marked as pinned.
        __always_inline static auto is_pinned(epoch&& e) -> bool
        {
            return (e.data & 1ul) == 1;
        }
        
        // epoch::pinned()
        // Returns the same epoch, but marked as pinned.
        __always_inline static auto pinned(epoch&& e) -> epoch
        {
            return epoch{ e.data | 1ul };
        }

        // epoch::unpinned()
        // Returns the same epoch, but marked as unpinned.
        __always_inline static auto unpinned(epoch&& e) -> epoch
        {
            return epoch{ e.data & ~1ul };
        }

        // epoch::successor()
        // Returns the successor epoch.
        // 
        // Successor epoch marked as pinned iff the previous was as well.
        __always_inline static auto successor(epoch&& e) -> epoch
        {
            return epoch{ e.data + 2 };
        }   

        // epoch::get()
        // Get the current epoch value.
        __always_inline auto get() const noexcept -> usize_t
        {
            return this->data;
        }

    private:
        epoch() : data{0} {}
        epoch(usize_t init) : data{init} {}
    };

    // An atomic value that holds an `epoch`.
    class atomic_epoch
    {
        // since `epoch` is represented as a usize,
        // `atomic_epoch` is represented as an atomic_usize 
        atomic_usize_t data;
    
    public:
        atomic_epoch(atomic_epoch const&)            = delete;
        atomic_epoch& operator=(atomic_epoch const&) = delete;

        atomic_epoch(atomic_epoch&&)            = delete;
        atomic_epoch& operator=(atomic_epoch&&) = delete;

        // atomic_epoch::make()
        // Create a new atomic epoch.
        __always_inline static auto make(epoch&& e) -> atomic_epoch
        {
            return atomic_epoch{ e.get() };
        }

        // atomic_epoch::load()
        // Loads a value from the atomic epoch.
        __always_inline auto load(std::memory_order order) -> epoch
        {
            auto const d = std::atomic_load_explicit(&this->data, order);
            return epoch::with_value(d);
        }

        // atomic_epoch::store()
        // Stores a value into the atomic epoch.
        __always_inline auto store(epoch&& e, std::memory_order order) -> void
        {
            std::atomic_store_explicit(&this->data, e.get(), order);
        }

        // atomic_epoch::compare_and_swap()
        // Stores a value into the atomic epoch if the current value is the same as `current`.
        //
        // The return value is always the previous value. If it is equal to
        // `current`, then the value is updated.
        __always_inline auto compare_and_swap(epoch&& current, epoch&& next, std::memory_order order) -> epoch
        {
            auto prev = current.get();
            std::atomic_compare_exchange_strong_explicit(
                &this->data, 
                &prev, 
                next.get(), 
                ordering_success(order), 
                ordering_failure(order));

            return epoch::with_value(prev);
        }

    private:
        atomic_epoch(usize_t init) : data{init} {}
    };
}

#endif // EPIC_EPOCH_H