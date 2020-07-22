// local.hpp

#ifndef EPIC_LOCAL_H
#define EPIC_LOCAL_H

#include "cell.hpp"
#include "guard.hpp"
#include "epoch.hpp"
#include "collector.hpp"
#include "type_alias.hpp"

#include <memory>
#include <lowlock/list.hpp>

namespace epic
{
    class bag;

    // epic::local
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
        local(collector& c);

        // local::register_handle()
        // Register a new `local` in the `global` associated with
        // the provided `collector` instance.
        static auto register_handle(collector& c) -> local_handle;

        // local::get_global()
        auto get_global() const -> global&;

        // local::get_collector()
        // Returns a reference to the `collector` instance in which this `local` resides.
        auto get_collector() const -> collector const&;

        // local::get_epoch()
        auto get_epoch() const -> epoch;

        // local::is_pinned()
        // Returns `true` if the current participant is pinned.
        auto is_pinned() const -> bool;

        // local::defer()
        // Adds the deferred function `d` to the thread-local bag.
        auto defer(deferred&& d, guard& g) -> void;

        // local::flush()
        // Flush all local deferred functions to the global cache,
        // and trigger a global collection.
        auto flush(guard& g) -> void;

        // local::pin()
        // Pins the `local` instance.
        auto pin() -> guard;
        
        // local::unpin()
        // Unpins the `local` instance.
        auto unpin() -> void;

        // local::repin()
        // Unpins and then pins the `local`.
        auto repin() -> void;

        // local::acquire_handle()
        // Increments the handle count.
        auto acquire_handle() -> void;

        // local::release_handle()
        // Decrements the handle count.
        auto release_handle() -> void;

        // local::finalize()
        // Removes the `local` instance from the global linked list.
        auto finalize() -> void;

        // local::entry_of()
        // Return a reference to this list element's embedded entry.
        static auto entry_of(local& l) -> lowlock::list_entry&;

        // local::element_of()
        // Given a reference to a list element's entry, return a reference to the element.
        static auto element_of(lowlock::list_entry& e) -> local&;
    };
}

#endif // EPIC_LOCAL_H