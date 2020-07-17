// deferred.hpp

#ifndef EPIC_DEFERRED_H
#define EPIC_DEFERRED_H

#include <functional>

namespace epic
{
    // A deferred function wrapper.
    //
    // TODO: implement inline optimization from crossbeam::epoch.
    class deferred
    {
        std::function<void()> fn;
    
    public:
        deferred()  = delete;
        ~deferred() = default;

        static auto make(std::function<void()>&& f) -> deferred
        {
            return deferred{std::move(f)};
        }

        auto call() -> void
        {
            fn();
        }

    private:
        deferred(std::function<void()>&& f) : fn{std::move(f)} {}
    };
}

#endif // EPIC_DEFERRED_H