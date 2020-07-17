// atomic.hpp

#ifndef EPIC_ATOMIC_H
#define EPIC_ATOMIC_H

#include <atomic>

namespace epic
{
    // An atomic pointer that can be safely shared between threads.
    //
    // The pointer must be properly aligned. Since it is aligned, a tag can
    // be stored in the least significant bits of the addres. For example,
    // the tag for a pointer to a sized type T should be less than:
    // 
    // Any method that loads the pointer must be passed a reference to an epic::guard.
    template <typename T>
    class atomic
    {
        std::atomic_size_t data;

    public:
        static size_t ALIGN;

        atomic() = delete;

        // static auto create(T init) -> atomic<T>
        // {
        //     return atomic<T>{init};
        // }

        static auto from_usize(size_t data) -> atomic<T>
        {
            return atomic<T>{data};
        }

        static auto null() -> atomic<T>
        {
            return atomic<T>{0};
        }

        atomic(atomic const&)            = delete;
        atomic& operator=(atomic const&) = delete;

        // atomic(atomic&&)            = delete;
        // atomic& operator=(atomic&&) = delete;

    private:
        atomic(T init) : data{init} {}
    };
}

#endif // EPIC_ATOMIC_H 