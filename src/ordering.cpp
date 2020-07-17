// ordering.cpp

#include <tuple>
#include <atomic>

namespace epic
{
    // strongest_failure_ordering()
    // Given ordering for the success case in a compare-exchange operation,
    // returns the strongest appropriate ordering for the failure case.
    auto strongest_failure_ordering(std::memory_order order) -> std::memory_order
    {
        return [=](){
            switch (order)
            {
                case std::memory_order_relaxed:
                case std::memory_order_release:
                    return std::memory_order_relaxed;
                case std::memory_order_acquire:
                case std::memory_order_acq_rel:
                    return std::memory_order_acquire;
                default:
                    return std::memory_order_seq_cst;
            }
        }();
    }

    // Memory orderings for compare-and-set operations.

    // ordering_success()
    auto ordering_success(std::memory_order order) -> std::memory_order
    {
        return order;
    }

    // ordering_success()
    auto ordering_success(std::pair<std::memory_order, std::memory_order> const &order_pair) -> std::memory_order
    {
        return std::get<0>(order_pair);
    }

    // ordering_failure()
    auto ordering_failure(std::memory_order order) -> std::memory_order
    {
        return strongest_failure_ordering(order);
    }

    // ordering_failure()
    auto ordering_failure(std::pair<std::memory_order, std::memory_order> const& order_pair) -> std::memory_order
    {
        return std::get<1>(order_pair);
    }
}