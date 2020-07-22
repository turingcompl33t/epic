// local.hpp

#ifndef EPIC_LOCAL_H
#define EPIC_LOCAL_H

#include "bag.hpp"
#include "cell.hpp"
#include "owned.hpp"
#include "guard.hpp"
#include "epoch.hpp"
#include "collector.hpp"
#include "type_alias.hpp"

#include <memory>
#include <lowlock/list.hpp>

namespace epic
{
    // local
    //
    // A participant in garbage collection.
    class local
    {
        // The number of pinnings after which the participant will 
        // execute some deferred functions from the global queue.
        static usize_t const PINNINGS_BETWEEN_COLLECT = 128;

        // An entry in the intrusive linked list of `local`s.
        lowlock::list_entry entry;

        // The local epoch.
        atomic_epoch local_epoch;

        // A reference to the global data.
        collector instance;

        // The local bag of deferred functions.
        std::unique_ptr<bag> deferreds;

        // The number of guards keeping this participant pinned.
        cell<usize_t> guard_count;

        // The number of active handles.
        cell<usize_t> handle_count;

        // The total number of pinnings performed.
        // This is an auxilliary counter that sometimes kicks off collection.
        cell<usize_t> pin_count;

    public:
        local(collector& c) 
            : entry{}
            , local_epoch{epoch{}}
            , instance{c}
            , deferreds{std::make_unique<bag>()}
            , guard_count{0}
            , handle_count{0}
            , pin_count{0}
        {}

        // local::register_local()
        // Register a new `local` in the `global` associated with
        // the provided `collector` instance.
        static auto register_local(collector& c) -> local_handle
        {
            // construct a new local instance on the heap
            auto* l = new local{c};

            // insert the new local into the global list of `local`s
            lowlock::list::push_front(
                c.instance->locals, local::entry_of(*l));

            // return a `local_handle` for the new `local`
            return local_handle{l};
        }

        __always_inline auto get_global() const -> global&
        {
            return *get_collector().instance;
        }

        // local::get_collector()
        // Returns a reference to the `collector` instance in which this `local` resides.
        __always_inline auto get_collector() const -> collector const&
        {
            return instance;
        }

        __always_inline auto get_epoch() const -> epoch
        {
            return local_epoch.load(std::memory_order_relaxed);
        }

        // local::is_pinned()
        // Returns `true` if the current participant is pinned.
        __always_inline auto is_pinned() const -> bool
        {
            return guard_count.get() > 0;
        }

        // local::defer()
        // Adds the deferred function `d` to the thread-local bag.
        auto defer(deferred&& d, guard& g) -> void
        {
            for (;;)
            {
                // Attempt to add the deferred function to the thread local bag.
                auto def = deferreds->try_push(std::move(d));
                if (def.has_value())
                {
                    // Push of the deferred to thread local bag failed
                    // because the bag is full, seal the bag and
                    // push it onto the global queue
                    
                    // Create a new bag and swap it with the full one.
                    auto new_bag = std::make_unique<bag>();
                    deferreds.swap(new_bag);
                    
                    get_global().push_bag(std::move(new_bag));

                    d = std::move(def.value());
                }
                else
                {
                    // Successfully pushed the function into thread local bag.
                    break;
                }
            }
        }

        // local::flush()
        // Flush all local deferred functions to the global cache,
        // and trigger a global collection.
        auto flush(guard& g) -> void
        {
            if (!deferreds->is_empty())
            {
                auto new_bag = std::make_unique<bag>();
                deferreds.swap(new_bag);

                get_global().push_bag(std::move(new_bag));
            }

            get_global().collect();
        }

        // local::pin()
        // Pins the `local` instance.
        auto pin() -> guard
        {
            auto g = guard{ this };

            auto const count = guard_count.get();
            guard_count.set(count + 1);

            if (0 == count)
            {
                // Previously, the gaurd count for this `local` was 0, 
                // so this participant becomes pinned in the current global epoch.

                auto global_epoch = get_global().global_epoch.load(std::memory_order_relaxed);
                auto new_epoch = global_epoch.pinned();

                // Store the new global epoch.
                local_epoch.store(new_epoch, std::memory_order_seq_cst);

                // Increment the local pin count.
                auto p_count = pin_count.get();
                pin_count.set(p_count + 1);

                // After every `PINNINGS_BETWEEN_COLLECT` try to 
                // advanced the epoch and collecting some garbage.
                if (0 == p_count % PINNINGS_BETWEEN_COLLECT)
                {
                    get_global().collect();
                }
            }

            return g;
        }
        
        // local::unpin()
        // Unpins the `local` instance.
        auto unpin() -> void
        {
            auto const count = guard_count.get();
            guard_count.set(count + 1);

            if (1 == count)
            {
                local_epoch.store(epoch{}, std::memory_order_release);

                if (0 == handle_count.get())
                {
                    finalize();
                }
            }
        }

        // local::repin()
        // Unpins and then pins the `local`.
        auto repin() -> void
        {
            auto const count = guard_count.get();

            // Update the local epoch if there is only one guard.
            if (1 == count)
            {
                auto l_epoch = local_epoch.load(std::memory_order_relaxed);
                auto g_epoch = get_global().global_epoch.load(std::memory_order_relaxed).pinned();

                // Update the local epoch only if the global epoch is greater.
                if (l_epoch != g_epoch)
                {
                    local_epoch.store(g_epoch, std::memory_order_release);
                }
            }
        }

        // local::acquire_handle()
        // Increments the handle count.
        auto acquire_handle() -> void
        {
            auto const count = handle_count.get();
            assert(count >= 1);
            handle_count.set(count + 1);
        }

        // local::release_handle()
        // Decrements the handle count.
        auto release_handle() -> void
        {
            auto g_count = guard_count.get();
            auto h_count = handle_count.get();
            
            assert(h_count >= 1);

            handle_count.set(h_count - 1);

            if (0 == g_count && 1 == h_count)
            {
                finalize();
            }
        }

        // local::finalize()
        // Removes the `local` instance from the global linked list.
        auto finalize() -> void
        {
            auto const g_count = guard_count.get();
            auto const h_count = handle_count.get();

            assert(0 == g_count);
            assert(0 == h_count);

            // Temporarily increment handle count.
            // This is required so that the following call to `pin()`
            // does not call `finalize()` again.
            handle_count.set(1);

            // Pin and move the local bag to the global queue.
            auto g = pin();
            get_global().push_bag(std::move(deferreds));

            handle_count.set(0);

            // Remove the entry for this local from the linked list.
            auto& global_list = get_global().locals;
            lowlock::list::remove_entry(global_list, entry);

            // Drop the reference to the global shared state.
            instance.release();
        }

        // local::entry_of()
        // Return a reference to this list element's embedded entry.
        static auto entry_of(local& l) -> lowlock::list_entry&
        {
            auto* entry_ptr = reinterpret_cast<lowlock::list_entry*>(
                reinterpret_cast<ptrdiff_t>(&l) + offsetof(local, entry));
            return *entry_ptr;
        }

        // local::element_of()
        // Given a reference to a list element's entry, return a reference to the element.
        static auto element_of(lowlock::list_entry& e) -> local&
        {
            auto* local_ptr = reinterpret_cast<local*>(
                reinterpret_cast<usize_t>(&e) - offsetof(local, entry));
            return *local_ptr;
        }
    };
}

#endif // EPIC_LOCAL_H