// bag.hpp

#ifndef EPIC_BAG_H
#define EPIC_BAG_H

#include <cstddef>
#include <optional>

#include "epoch.hpp"
#include "deferred.hpp"

namespace epic
{
    // the maxmimum number of objects a bag may contain
    // TODO: make 64 in non-debug build
    constexpr static size_t const MAX_OBJECTS = 4;

    // A bag of deferred functions.
    class bag
    {   
        // Is this bag sealed?
        bool sealed;

        // The current count of stored deferred functions.
        size_t count;

        // The epoch associated with this bag, once sealed.
        epoch sealed_epoch;

        // The inline array of deferred functions.
        std::array<deferred, MAX_OBJECTS> deferreds;
    
    public:
        bag();

        ~bag();

        // bag::is_empty()
        auto is_empty() const noexcept -> bool;

        // bag::is_expired()
        // Determines if it is safe to collect the given bag
        // with respect to the current global epoch.
        auto is_expired(epoch const& e) const noexcept -> bool;

        // bag::try_push()
        auto try_push(deferred&& def) -> std::optional<deferred>;

        // bag::seal()
        // Seals the bag with the given epoch.
        auto seal(epoch const& e) -> void;
    };
}

#endif // EPIC_BAG_H