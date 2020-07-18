// queue.hpp

#ifndef EPIC_QUEUE_H
#define EPIC_QUEUE_H

#include "../atomic.hpp"

namespace epic
{
    template <typename T>
    class node
    {   
        atomic<node<T>> next;
        T data;
    };

    template <typename T>
    class queue
    {
        atomic<node<T>> head;
        atomic<node<T>> tail;

    public:
        queue(queue const&) = delete;
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

        // queue::make()
        static auto make() -> queue<T>
        {
            return queue<T>{};
        }

    private:
        queue() 
            : head{std::move(atomic<node<T>>::null())}
            , tail{std::move(atomic<node<T>>::null())}
        {}
    };
}

#endif // EPIC_QUEUE_H