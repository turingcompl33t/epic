// queue.hpp

#ifndef EPIC_QUEUE_H
#define EPIC_QUEUE_H

#include "atomic.hpp"
#include "guard.hpp"

#include <optional>

namespace epic
{
    // node
    //
    // An individual node in the queue's underlying linked-list.
    //
    // Type `T` must support default construction because the `T`
    // may be uninitialized (e.g. for sentinel node of queue).
    //
    // Type `T` must support move construction because this is
    // how we take ownership of the stored data in the case that
    // the node is constructed with an initializer.
    template <typename T>
    class node
    {   
        // The pointer to the next node in the list.
        atomic<node<T>> next;

        // The slot in which a value of type `T` may be stored.
        //
        // A node<T> does not always contain a `T`. For instance,
        // the sentinel node in the queue never contains a value,
        // its slot is always empty. Other nodes (the general case)
        // start their lives with a push operation an contain a 
        // value until the node is popped. After that, such empty 
        // nodes get added to the collector for destruction.
        T data;

    public:
        // Default constructor initializes the `next` pointer to
        // null, and default constructs the internal data.
        node() 
            : next{atomic<node<T>>::null()}
            , data{}
        {}

        // Non-default constructor accepts initializer for internal data.
        node(T&& init) 
            : next{atomic<node<T>>::null()}
            , data{std::forward(init)}
        {}
    };

    // queue
    //
    // Michael-Scott lock-free queue.
    //
    // The representation used here is a singly-linked list,
    // with a sentinel node at the front. In general, the `tail`
    // pointer may lag behind the actual tail. Non-sentinel nodes
    // are either all `Data` or all `Blocked` (requests for data
    // from blocked threads).
    template <typename T>
    class queue
    {
        atomic<node<T>> head;
        atomic<node<T>> tail;

    public:
        // The default constructor first initialized both
        // head and tail pointers to null.
        queue() 
            : head{std::move(atomic<node<T>>::null())}
            , tail{std::move(atomic<node<T>>::null())}
        {
            // Construct a new node without internel data on the heap.
            auto sentinel = owned<node<T>>::make();

            // Grab a dummy guard.
            auto g = unprotected();

            // Convert the `owned` sentinel into a `shared` instance.
            auto shared_sentinel = owned<node<T>>::into_shared(std::move(sentinel), g);

            // Insert the sentinel into the queue.
            this->head.store(std::move(shared_sentinel), std::memory_order_relaxed);
            this->tail.store(std::move(shared_sentinel), std::memory_order_relaxed);
        }

        ~queue()
        {
            auto g = unprotected();

            // Pop remaining nodes off the queue.
            while (try_pop(g).has_value()) {}

            {
                // Destroy the remaining sentinel node.
                auto sentinel = this->head.load(std::memory_order_relaxed, g);
                auto owned_sentinel = sentinel.into_owned();
            }
        }

        queue(queue const&)            = delete;
        queue& operator=(queue const&) = delete;

        queue(queue&& q) 
            : head{std::move(q.head)}
            , tail{std::move(q.tail)}
        {}

        queue& operator=(queue&& q)
        {
            if (&q == this)
            {
                this->head = std::move(q.head);
                this->tail = std::move(q.tail);
            }

            return *this;
        }

        // queue::push()
        // Pushes `t` on the back of the queue, 
        // possibly waking up threads blocked on `pop_front()`
        auto push(T&& t, guard& g) -> void
        {
            // Construct a new node that takes ownership of the pushed data.
            auto owned_node = owned<node<T>>::make(std::forward(t));

            // Convert the owned node into a `shared` instance.
            auto shared_node = owned<node<T>>::into_shared(std::move(owned_node), g);

            for (;;)
            {
                // We push onto the tail, so we start optimistically by looking there first. 
                auto queue_tail = this->tail.load(std::memory_order_acquire, g);

                // Attempt to push onto the tail snapshot; fails if `tail.next` has changed.
                if (push_internal(queue_tail, shared_node, g))
                {
                    break;
                }
            }
        }

        // queue::try_pop()
        // Attempts to remove from the front of the queue.
        //
        // Returns empty optional (std::nullopt) if queue is
        // observed to be empty.
        auto try_pop(guard& g) -> std::optional<T>
        {
            // TODO
        }

        // queue::try_pop_if()
        // Attempts to dequeue from the front of the queue, if the
        // item satisfies the given predicate.
        //
        // Returns empty optional (std::nullopt) if the queue is
        // observed to be empty, or the item at the head of the
        // queue does not satisfy the given predicate.
        auto try_pop_if(
            std::function<bool(T&)>&& predicate, 
            guard& g) -> std::optional<T>
        {
            // TODO
        }

    private:
        // queue::push_internal()
        // Attempts to atomically place `new_node` into the `next`
        // pointer of `onto`, returning `true` on success. The 
        // queue's `tail` pointer may be updated. 
        auto push_internal(
            shared<node<T>>& onto, 
            shared<node<T>>& new_node,
            guard& g) -> bool
        {
            // Determine if `onto` is actually the tail of the queue.
            auto& o = *onto;
            auto next = o.next.load(std::memory_order_acquire, g);

            if (next.as_ref())
            {
                // The node we though was the tail has a valid `next`
                // pointer, so help out by moving the tail pointer along.
                this->tail.compare_and_set(onto, next, std::memory_order_release, g);
                return false;
            }
            else
            {
                // Otherwise, it appears that `onto` is actually the tail of the queue.
                // Attempt to link `new_node` into the list.
                auto result = o.next.compare_and_set(
                    shared<node<T>>::null(), 
                    new_node, 
                    std::memory_order_release, 
                    g);

                if (result.has_value())
                {
                    // Try to move the tail pointer forward to reflect actual new tail.
                    this->tail.compare_and_set(onto, new_node, std::memory_order_release, g);
                }

                return result.has_value();
            }
        }

        // queue::pop_internal()
        auto pop_internal(guard& g) -> void
        {
            // TODO
        }

        // queue::pop_if_internal()
        auto pop_if_internal(std::function<bool(T&)>&& predicate, guard& g) -> void
        {
            // TODO
        }
    };
}

#endif // EPIC_QUEUE_H