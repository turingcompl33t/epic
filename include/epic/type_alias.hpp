// type_alias.hpp

#ifndef EPIC_TYPE_ALIAS_H
#define EPIC_TYPE_ALIAS_H

#include <atomic>
#include <climits>
#include <cstddef>

namespace epic
{
    using usize_t = unsigned long int;
    using isize_t = signed long int;

    using atomic_usize_t = std::atomic_ulong;

    constexpr static auto const USIZE_MIN = 0;
    constexpr static auto const USIZE_MAX = ULONG_MAX;

    constexpr static auto const ISIZE_MIN = LONG_MIN;
    constexpr static auto const ISIZE_MAX = LONG_MAX;
}

#endif // EPIC_TYPE_ALIAS_H