// ordering.hpp

#ifndef EPIC_ORDERING_H
#define EPIC_ORDERING_H

#include <tuple>
#include <atomic>

namespace epic
{
    // strongest_failure_ordering()
    // Given ordering for the success case in a compare-exchange operation,
    // returns the strongest appropriate ordering for the failure case.
    auto strongest_failure_ordering(std::memory_order order) -> std::memory_order;

    // Memory orderings for compare-and-set operations.

    // ordering_success()
    auto ordering_success(std::memory_order order) -> std::memory_order;

    // ordering_success()
    auto ordering_success(std::pair<std::memory_order, std::memory_order> const &order_pair) -> std::memory_order;

    // ordering_failure()
    auto ordering_failure(std::memory_order order) -> std::memory_order;

    // ordering_failure()
    auto ordering_failure(std::pair<std::memory_order, std::memory_order> const& order_pair) -> std::memory_order;
}

#endif // EPIC_ORDERING_H