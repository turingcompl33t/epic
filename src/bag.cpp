// bag.cpp

#include <epic/bag.hpp>

#include <cassert>

namespace epic
{
    // The no-op function used to initialize a new bag.
    static auto no_op() -> void {}

    bag::bag() 
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

    bag::~bag()
    {
        // call all the deferred functions in bag on drop
        for (auto& d : deferreds)
        {
            d.call();
        }
    }

    // bag::is_empty()
    auto bag::is_empty() const noexcept -> bool
    {
        return 0 == count;
    }

    // bag::is_expired()
    // Determines if it is safe to collect the given bag
    // with respect to the current global epoch.
    auto bag::is_expired(epoch const& e) const noexcept -> bool
    {
        // It is a logic error to attempt collection on unsealed bag.
        assert(sealed);
        return e.wrapping_sub(sealed_epoch) >= 2;
    }

    // bag::try_push()
    auto bag::try_push(deferred&& def) -> std::optional<deferred>
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
    auto bag::seal(epoch const& e) -> void
    {
        // On seal, we stored the epoch at which this bag was sealed.
        sealed_epoch = e;
        sealed       = true;
    }
}