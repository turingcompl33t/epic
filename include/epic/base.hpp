// base.hpp

#ifndef EPIC_BASE_H
#define EPIC_BASE_H

#include <atomic>
#include <tuple>
#include <cstddef>
#include <cassert>

#include "pointer.hpp"

namespace epic
{
    // The error returned on failed compare-and-set operation.
    // TODO
    struct compare_and_set_error
    {

    };

    // trailing_zeros()
    __always_inline auto trailing_zeros(size_t const n) -> int
    {
        assert(n != 0);
        return __builtin_ctz(n);
    }
    
    // low_bits()
    // Returns a bitmask containing the unused least significant
    // bits of an aligned pointer to T.
    //
    // TODO: constrain via concept??
    template <typename T>
    inline auto low_bits() -> size_t
    {
        auto const align = pointable<T>::alignment();
        return (1 << trailing_zeros(align)) - 1; 
    }

    // ensure_aligned()
    // Throws std::runtime_error if pointer not aligned.
    template <typename T>
    inline auto ensure_aligned(size_t const raw) -> void
    {
        auto const r = raw & low_bits<T>();
        if (r != 0)
        {
            throw std::runtime_error{"unaligned pointer"};
        }
    }

    // compose_tag()
    // Given a tagged pointer, returns the same pointer but
    // tagged with the specified tag.
    template<typename T>
    inline auto compose_tag(size_t const data, size_t const tag) -> size_t
    {
        return (data & !low_bits<T>()) | (tag & low_bits<T>());
    }

    // decompose_tag()
    // Decomposes a tagged pointer into the pointer and the tag.
    template <typename T>
    inline auto decompose_tag(size_t const data) -> std::pair<size_t, size_t>
    {
        return std::make_pair(data & ~low_bits<T>(), data & low_bits<T>());
    }
}

#endif // EPIC_BASE_H