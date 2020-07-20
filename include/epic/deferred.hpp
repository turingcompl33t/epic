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
        deferred(std::function<void()>&& f) 
            : fn{std::move(f)} {}
        
        ~deferred() = default;

        deferred(deferred const&)            = delete;
        deferred& operator=(deferred const&) = delete;

        deferred(deferred&& d) : fn{std::move(d.fn)} {}
        deferred& operator=(deferred&& d)
        {
            if (&d != this)
            {
                this->fn = std::move(d.fn);
            }

            return *this;
        }

        // deferred::call()
        // Invoke the deferred function.
        auto call() -> void
        {
            fn();
        }
    };
}

#endif // EPIC_DEFERRED_H