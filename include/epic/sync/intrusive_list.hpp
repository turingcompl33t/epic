// intrusive_list.hpp

#ifndef EPIC_INTRUSIVE_LIST_H
#define EPIC_INTRUSIVE_LIST_H

#include "../atomic.hpp"

namespace epic
{
    // List entry.

    class list_entry
    {
        // The next entry in the linked-list.
        // If the tag is 1, this entry is marked as deleted.
        atomic<list_entry> next;

    public:
        // list_entry::empty()
        // Returns the empty entry.
        static auto empty() -> list_entry
        {
            return list_entry{};
        }

        // list_entry::mark_removed()
        // Marks this entry as deleted, deferring the actual
        // deallocation to a later iteration.
        auto mark_removed(guard& g) -> void
        {
            this->next.fetch_or(1, std::memory_order_release, g);
        }
    
    private:
        list_entry() 
            : next{std::move(atomic<list_entry>::null())} {}
    };

    // Instrusive list.

    template <typename T>
    class intrusive_list
    {
        atomic<list_entry> head;

    public:
        intrusive_list(intrusive_list const&)            = delete;
        intrusive_list& operator=(intrusive_list const&) = delete;

        intrusive_list(intrusive_list&& it) 
            : head{std::move(it.head)}

        intrusive_list& operator=(intrusive_list&& it)
        {
            if (&it != this)
            {
                this->head = std::move(it.head);
            }

            return *this;
        }

        // intrusive_list::make()
        // Returns a new, empty linked list.
        static auto make() -> intrusive_list<T>
        {
            return intrusive_list{};
        }

        // intrusive_list::insert()
        auto insert(shared<T> container, guard& g) 
        {
            // Insert right after head i.e. at beginning of list.
            auto& = this->head;
            // Get the intrusively stored list_entry of the new element to insert.
            // Make a shared pointer to that entry
        }

    private:
        intrusive_list() 
            : head{std::move(atomic<list_entry>::null())} {}
    };

    template <typename T>
    class list_iter
    {
        // The guard that protects iteration.
        guard& guard_;
        
        // Pointer to the predecessor of current entry.
        atomic<list_entry> prev;

        // The current entry.
        shared<list_entry> curr;

        // The list head, needed for restarting iteration.
        atomic<list_entry> head;
    };
}

#endif // EPIC_INTRUSIVE_LIST_H