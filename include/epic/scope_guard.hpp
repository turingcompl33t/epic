// scope_guard.hpp

#ifndef EPIC_SCOPE_GUARD_H
#define EPIC_SCOPE_GUARD_H

#include <functional>

namespace epic
{
    // scope_guard
    //
    // A highly-oversimplified scope guard implementation.

    class scope_guard
    {
        std::function<void()> fn;

    public:
        scope_guard(std::function<void()>&& fn_)
            : fn{std::move(fn_)}
        {}

        ~scope_guard()
        {
            fn();
        }
    };
}

#endif // EPIC_SCOPE_GUARD_H