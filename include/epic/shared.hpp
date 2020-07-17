// shared.hpp

#ifndef EPIC_SHARED_H
#define EPIC_SHARED_H

#include <cstddef>

namespace epic
{
    template <typename T>
    class shared
    {
    public:
        // shared::into_usize()
        auto into_usize() -> size_t
        {
            return 0;
        }

        // shared::from_usize()
        static auto from_usize(size_t data) -> shared<T>
        {
            return shared<T>{};
        }
    };
};

#endif // EPIC_SHARED_H