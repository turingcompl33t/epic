// intrusive_list.hpp

#ifndef EPIC_INTRUSIVE_LIST_H
#define EPIC_INTRUSIVE_LIST_H

#include <cassert>

#include "guard.hpp"
#include "atomic.hpp"

namespace epic
{
    // list_entry
    //
    // A single entry in the intrusive linked-list.
    class list_entry
    {
        // The next entry in the linked-list.
        // If the tag is 1, this entry is marked as deleted.
        atomic<list_entry> next;

    public:
        // Default constructor initializes with null pointer.
        list_entry() 
            : next{std::move(atomic<list_entry>::null())} 
        {}

        list_entry(list_entry const&)            = delete;
        list_entry& operator=(list_entry const&) = delete;

        list_entry(list_entry&& other) 
            : next{std::move(other.next)}
        {}

        list_entry& operator=(list_entry&& rhs)
        {
            if (&rhs != this)
            {
                this->next = std::move(rhs.next);
            }

            return *this;
        }

        // list_entry::mark_removed()
        // Marks this entry as deleted, deferring the actual
        // deallocation to a later iteration.
        auto mark_removed(guard& g) -> void
        {
            this->next.fetch_or(1, std::memory_order_release, g);
        }
    };

    // intrusive_list_iter
    //
    // A custom iterator over an atomic, singly-linked list.
    template <typename T>
    class intrusive_list_iterator
    {
        // The guard that protects iteration.
        guard& guard_;
        
        // Pointer to the predecessor of current entry.
        atomic<list_entry> prev;

        // The current entry.
        shared<list_entry> curr;

        // The list head, needed for restarting iteration.
        atomic<list_entry> head;

    public:
        // TODO
        intrusive_list_iterator() {}
    };

    // intrusive_list
    //
    // An atomic, singly-linked intrusive list.
    template <typename T>
    class intrusive_list
    {
        atomic<list_entry> head;

    public:
        using iterator = intrusive_list_iterator<T>;

        // Default constructor initializes head with null pointer.
        intrusive_list() 
            : head{std::move(atomic<list_entry>::null())} 
        {}

        // Destructor 
        ~intrusive_list()
        {
            // grab a reference to the dummy guard
            auto g = guard::unprotected();
            
            // loading the `atomic` head produces a `shared` for list_entry
            auto current = head.load(std::memory_order_relaxed, g);

            // TODO: would like to handle this with a std::optional<T&> from
            // within epic::shared<T>, but obviously we can't do that yet
            while (!current.is_null())
            {
                auto successor = current->next.load(std::memory_order_relaxed, g);
                // Verify that all elements have been removed from the list
                assert(successor.tag() == 1);

                // TODO: finalize()

                current = successor;
            }
        }   

        intrusive_list(intrusive_list const&)            = delete;
        intrusive_list& operator=(intrusive_list const&) = delete;

        intrusive_list(intrusive_list&& other) 
            : head{std::move(other.head)}
        {}

        intrusive_list& operator=(intrusive_list&& rhs)
        {
            if (&rhs != this)
            {
                this->head = std::move(rhs.head);
            }

            return *this;
        }

        // intrusive_list::insert()
        // Insert the entry embedded in `container` into the list.
        auto insert(shared<T>&& container, guard& g) -> void
        {
            // Insert right after head i.e. at beginning of list.
            // auto& = this->head;

            // TODO
            // Get the intrusively stored list_entry of the new element to insert.
            // Make a shared pointer to that entry
        }

        auto begin() -> iterator
        {
            // TODO
            return iterator{};
        }

        auto end() -> iterator
        {
            // TODO
            return iterator{};
        }
    };
}

#endif // EPIC_INTRUSIVE_LIST_H