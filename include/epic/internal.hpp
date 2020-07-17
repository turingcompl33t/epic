// internal.hpp

#ifndef EPIC_INTERNAL_H
#define EPIC_INTERNAL_H

#include <array>
#include <memory>
#include <cstddef>
#include <optional>

#include "deferred.hpp"

// the maxmimum number of objects a bag may contain
constexpr static size_t const MAX_OBJECTS = 4;

static auto no_op() -> void {}

namespace epic
{
    // A static bag and the associated epoch.
    class sealed_bag
    {
        
    };

    // A bag of deferred functions.
    class bag
    {
        std::array<deferred, MAX_OBJECTS> deferreds;
        size_t count;
    
    public:
        // bag::make()
        static auto make() -> std::unique_ptr<bag>
        {
            auto* raw = new bag{};
            return std::unique_ptr<bag>{raw};
        }

        ~bag()
        {
            // call all the deferred functions in bag on drop
            for (auto& d : this->deferreds)
            {
                auto no_op_def = deferred::make(no_op);
                auto owned_def = std::move(d);
            }
        }

        // bag::is_empty()
        auto is_empty() const noexcept -> bool
        {
            return 0 == this->count;
        }

        // bag::try_push()
        auto try_push(deferred&& def) -> std::optional<deferred>
        {
            if (this->count < MAX_OBJECTS)
            {
                this->deferreds[this->count] = std::move(def);
                this->count++;
                return std::nullopt;
            }
            else
            {
                return std::make_optional<deferred>(std::move(def));
            }
        }

    private:
        bag() : 
        deferreds {
                deferred::make(no_op),
                deferred::make(no_op),
                deferred::make(no_op),
                deferred::make(no_op)
            },
        count{0} {}
    };
}

#endif // EPIC_INTERNAL_H