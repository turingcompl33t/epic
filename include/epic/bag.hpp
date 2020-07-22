// bag.hpp

#ifndef EPIC_BAG_H
#define EPIC_BAG_H

#include <memory>
#include <cstddef>
#include <optional>
#include <stdexcept>

#include "epoch.hpp"
#include "deferred.hpp"

// the maxmimum number of objects a bag may contain
// TODO: make 64 in non-debug build
constexpr static size_t const MAX_OBJECTS = 4;

static auto no_op() -> void {}

namespace epic
{
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
        bag() 
        : sealed{false}
        , count{0}
        , sealed_epoch{}
        , deferreds 
        {
            deferred{no_op},
            deferred{no_op},
            deferred{no_op},
            deferred{no_op}
        } {}

        ~bag()
        {
            // call all the deferred functions in bag on drop
            for (auto& d : deferreds)
            {
                d.call();
            }
        }

        // bag::is_empty()
        auto is_empty() const noexcept -> bool
        {
            return 0 == count;
        }

        // bag::is_expired()
        // Determines if it is safe to collect the given bag
        // with respect to the current global epoch.
        auto is_expired(epoch const& e) const noexcept -> bool
        {
            // It is a logic error to attempt collection on unsealed bag.
            assert(sealed);
            return e.wrapping_sub(sealed_epoch) >= 2;
        }

        // bag::try_push()
        auto try_push(deferred&& def) -> std::optional<deferred>
        {
            if (sealed)
            {
                throw std::runtime_error{"Attempt to push into a sealed bag"};
            }

            if (this->count < MAX_OBJECTS)
            {
                deferreds[count++] = std::move(def);
                return std::nullopt;
            }
            else
            {
                return std::make_optional<deferred>(std::move(def));
            }
        }

        // bag::seal()
        // Seals the bag with the given epoch.
        auto seal(epoch const& e) -> void
        {
            // On seal, we stored the epoch at which this bag was sealed.
            sealed_epoch = e;
            sealed       = true;
        }
    };
}

#endif // EPIC_BAG_H