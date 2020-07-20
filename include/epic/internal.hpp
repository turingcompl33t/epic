// internal.hpp

#ifndef EPIC_INTERNAL_H
#define EPIC_INTERNAL_H

#include <array>
#include <memory>
#include <cstddef>
#include <optional>

#include "epoch.hpp"
#include "deferred.hpp"
#include "sync/queue.hpp"
#include "sync/intrusive_list.hpp"

// the maxmimum number of objects a bag may contain
// TODO: make 64 in non-debug build
constexpr static size_t const MAX_OBJECTS = 4;

static auto no_op() -> void {}

namespace epic
{
    class bag;

    // A static bag and the associated epoch.
    class sealed_bag
    {
        std::unique_ptr<bag> bag_;
        epoch                epoch_;

    public:
        sealed_bag() = delete;

        // sealed_bag::make()
        // Construct a new sealed bag from an existing bag and associated epoch.
        static auto make(std::unique_ptr<bag>&& bag_ptr, epoch const& e) -> sealed_bag
        {
            return sealed_bag{std::move(bag_ptr), e};
        }

        // sealed_bag::is_expired()
        // Determines if it is safe to drop the bag w.r.t. current global epoch.
        auto is_expired(epoch const& global_epoch) -> bool
        {
            // A pinned participant can witness at most one epoch advancement.
            // Therefore, any bag that is within one epoch of the current one cannot
            // be safely destroyed yet.
            return global_epoch.wrapping_sub(this->epoch_) >= 2;
        }

    private:
        sealed_bag(std::unique_ptr<bag>&& bag_ptr, epoch const& e) 
            : bag_{std::move(bag_ptr)}, epoch_{e} {}
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
            return std::unique_ptr<bag>{new bag{}};
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

        // bag::seal()
        // Seals the bag with the given epoch.
        static auto seal(
            std::unique_ptr<bag>&& bag_ptr, 
            epoch const& e) -> sealed_bag
        {
            // on seal, the sealed_bag takes ownership of the bag and its items

            return sealed_bag::make(std::move(bag_ptr), e);
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

    // global
    //
    // The global data for a collector instance.
    class global
    {
        // Number of bags to destroy.
        static usize_t const COLLECT_STEPS = 8;

        // The intrusive linked list of `local`s.
        intrusive_list<local> locals;

        // The global queue of deferred functions.
        queue<sealed_bag> deferred_functions;

        // The global epoch.
        atomic_epoch global_epoch;

    public:

        // global::make()
        // Constructs a new global data instance for a collector.
        static auto make() -> std::shared_ptr<global>
        {
            return std::make_shared<global>();
        }

    private:
        global() 
            : locals{std::move(intrusive_list<local>::make())}
            , deferred_functions{std::move(queue<sealed_bag>::make())}
            , global_epoch{std::move(atomic_epoch::make(epoch::starting()))}
        {}
    };

    // local
    //
    // A participant in garbage collection.
    class local
    {
        static usize_t const PINNINGS_BETWEEN_COLLECT = 128;

        // An entry in the intrusive linked list of `local`s
        list_entry entry;

        // The local epoch.
        atomic_epoch local_epoch;

        // A reference to the global data.
        collector& instance;

        // The local bag of deferred functions.
        std::unique_ptr<bag> deferreds;

        // The number of guards keeping this participant pinned.
        usize_t guard_count;

        // The number of active handles.
        usize_t handle_count;

        // The total number of pinnings performed.
        //
        // This is an auxilliary counter that sometimes kicks off collection.
        usize_t pin_count;

    public:

        // local::get_global()
        // Returns a reference to the `global` instance in which the `local` resides.
        __always_inline auto get_global() const -> global&
        {
            return this->instance.global_as_ref();
        }

        // local::get_collector()
        // Returns a reference to the `collector` instance in which this `local` resides.
        __always_inline auto get_collector() const -> collector&
        {
            return this->instance;
        }

        // local::is_pinned()
        // Returns `true` if the current participant is pinned.
        __always_inline auto is_pinned() const -> bool
        {
            return this->guard_count > 0;
        }

        // local::defer()
        // Adds the deferred function `d` to the thread-local bag.
        auto defer(deferred&& d, guard& g) const -> void
        {
            // TODO
        }

        // local::flush()
        // TODO
        auto flush(guard& g) const -> void
        {
            // TODO
        }

        // local::pin()
        // Pins the `local`
        auto pin() const -> guard
        {
            // TODO
        }
        
        // local::unpin()
        // Unpins the `local`.
        auto unpin() const -> void
        {
            // TODO
        }

        // local::repin()
        // Unpins and then pins the `local`.
        auto repin() const -> void
        {
            // TODO
        }

        // local::acquire_handle()
        auto acquire_handle() const -> void
        {
            // TODO
        }

        // local::release_handle()
        auto release_handle() const -> void
        {
            // TODO
        }

        // local::entry_of()
        // Return a reference to this list element's embedded entry.
        static auto entry_of(local& l) -> list_entry&
        {
            auto* entry_ptr = reinterpret_cast<list_entry*>(
                reinterpret_cast<usize_t>(&l) + offsetof(local, entry));
            return *entry_ptr;
        }

        // local::element_of()
        // Given a reference to a list element's entry, return a reference to the element.
        static auto element_of(list_entry& e) -> local&
        {
            auto* local_ptr = reinterpret_cast<local*>(
                reinterpret_cast<usize_t>(&e) - offsetof(local, entry));
            return *local_ptr;
        }

        // local::finalize()
        // Called when the entry is unlinked from the list.
        static auto finalize(list_entry& e, guard& g) -> void
        {
            // TODO
        }
    };
}

#endif // EPIC_INTERNAL_H