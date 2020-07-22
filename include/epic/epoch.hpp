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
        epoch() : data{} {}
        
        epoch(epoch const& e) : data{e.data} {}

        epoch& operator=(epoch const& e)
        {
            this->data = e.data;
            return *this;
        }

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

        // epoch::with_value()
        // Returns a new epoch with the given value.
        static auto with_value(usize_t v) -> epoch
        {
            return epoch{v};
        }

        // epoch::wrapping_sub()
        // Returns the number of epochs `this` is ahead of `other`.
        //
        // Internally, epochs are represented as numbers in the range 
        // (ISIZE_MIN / 2) .. (ISIZE_MAX / 2), so the returned distance
        // will also be within this interval.
        auto wrapping_sub(epoch const& other) const -> isize_t
        {
            auto const masked = (other.data & ~1ul);
            return static_cast<isize_t>(this->data - masked) >> 1;
        }

        // epoch::is_pinned()
        // Return `true` if the epoch is marked as pinned.
        __always_inline auto is_pinned() const -> bool
        {
            return (this->data & 1ul) == 1;
        }
        
        // epoch::pinned()
        // Returns the same epoch, but marked as pinned.
        __always_inline auto pinned() const -> epoch
        {
            return epoch{ this->data | 1ul };
        }

        // epoch::unpinned()
        // Returns the same epoch, but marked as unpinned.
        __always_inline auto unpinned() const -> epoch
        {
            return epoch{ this->data & ~1ul };
        }

        // epoch::successor()
        // Returns the successor epoch.
        // 
        // Successor epoch marked as pinned iff the previous was as well.
        __always_inline auto successor() const -> epoch
        {
            return epoch{ this->data + 2 };
        }   

        // epoch::get()
        // Get the current epoch value.
        __always_inline auto get() const noexcept -> usize_t
        {
            return this->data;
        }

    private:
        epoch(usize_t init) : data{init} {}
    };

    // An atomic value that holds an `epoch`.
    class atomic_epoch
    {
        // since `epoch` is represented as a usize,
        // `atomic_epoch` is represented as an atomic_usize 
        atomic_usize_t data;
    
    public:
        atomic_epoch(epoch const& e) 
            : data{e.get()} {}

        atomic_epoch(atomic_epoch const&)            = delete;
        atomic_epoch& operator=(atomic_epoch const&) = delete;

        atomic_epoch(atomic_epoch&& e) 
            : data{e.data.load(std::memory_order_acquire)}
        {}

        atomic_epoch& operator=(atomic_epoch&& e)
        {
            if (&e != this)
            {
                auto const val = e.data.load(std::memory_order_acquire);
                this->data.store(val, std::memory_order_release);
            }

            return *this;
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
        __always_inline auto store(epoch const& e, std::memory_order order) -> void
        {
            std::atomic_store_explicit(&this->data, e.get(), order);
        }

        // atomic_epoch::compare_and_swap()
        // Stores a value into the atomic epoch if the current value is the same as `current`.
        //
        // The return value is always the previous value. If it is equal to
        // `current`, then the value is updated.
        __always_inline auto compare_and_swap(
            epoch const& current, 
            epoch const& next, 
            std::memory_order order) -> epoch
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
    };
}

#endif // EPIC_EPOCH_H