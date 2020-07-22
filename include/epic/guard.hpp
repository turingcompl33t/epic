// guard.hpp

#ifndef EPIC_GUARD_H
#define EPIC_GUARD_H

#include "shared.hpp"
#include "deferred.hpp"

namespace epic
{   
    class local;

    // guard
    // 
    // A guard that keeps the current thread pinned.
    //
    // Pinning
    //
    // The current thread is pinned by calling epic::pin() 
    // which returns a new `guard`. When the guard goes 
    // out of scope, the calling thread is automatically unpinned.
    //
    // Pointers on the Stack
    //
    // Holding an outstanding `guard` allows us to create pointers
    // on the stack to heap-allocated objects.
    //
    // Multiple Guards
    //
    // Pinning is reentrant and it is perfectly safe to create multiple
    // `guard`s. In this case, the thread is actually only pinned on
    // the creation of the first `guard` and unpinned when the last
    // `guard` falls out of scope.
    class guard
    {
          local const* local_ptr;
        
    public:
        // The default constructor has the same effect as guard::unprotected.
        guard();

        // The constructor that accepts a valid pointer to a `local` produces
        // a guard that is capable of keeping the calling thread pinned.
        guard(local const* local_ptr_);

        // If the guard is not a dummy guard produced by guard::unprotected,
        // the destructor for a `guard` unpins the thread that it has pinned.
        ~guard();

        // guard::defer()
        // Stores a function so that it will be executed at some point
        // after all currently pinned threads are unpinned.
        //
        // This method stores the function `f` into the thread-local
        // (or handle-local) cache. If this cache becomes full, some
        // functions from the local cache are moved into the global cache.
        // At the same time, some functions from both local and global
        // caches may be executed in order to incrementally clean up
        // the caches as they fill up.
        //
        // No guarantee is made as to when the function `f` will be executed.
        // The only guarantee is that it will not be executed until all
        // currently pinned threads are unpinned. In theory, `f` might
        // never run at all, but the epoch-based garbage collection scheme
        // will make an effort to execute it reasonably soon.
        //
        // If this method is called from a dummy guard produced by a call
        // to epic::unprotected(), the function is executed immediately.
        auto defer(std::function<void()>&& f) -> void;

        // guard::defer_destroy()
        // Stores a destructor for an object so that it can be deallocated
        // at some point after all currently pinned threads are unpinned.
        //
        // This method first stores the destructor into the thread-local 
        // cache of deferred functions. If this cache becomes full, some 
        // destructors are moved into the global cache. At the same time, 
        // some destructors from both local and global caches may be executed
        // in order to incrementally clean up the caches as they fill.
        //
        // There is no guarantee as to exactly when the destructor will be called.
        // The only guarantee is that this won't happen until all currently pinned
        // threads are unpinned. In theory, the destructor might never run, but
        // the epoch-based garbage collection scheme makes an effort to ensure
        // that it does reasonably soon.
        template <typename T>
        auto defer_destroy(shared<T>&& ptr) const -> void;

        // guard::flush()
        // Clears the thread-local cache of functions by executing them
        // or moving them to the global cache.
        //
        // Call this method after deferring execution of a function if 
        // you want to make sure that it executes as soon as possible.
        // Flushing will ensure that the deferred function is moved to 
        // the global cache so that any thread in the program has the 
        // opportunity to take the function and execute it.
        //
        // If this method is called from a dummy guard produced by epic::unprotected(),
        // it is a no-op.
        auto flush() -> void;

        // guard::repin()
        // Unpins and then immediately repins the thread.
        //
        // This method is useful when you don't want to delay the advancement of
        // the global epoch by holding an old epoch. For safety, you should not
        // maintain any guard-based reference across the call. The thread will
        // only be repinned if this guard is the only active guard for the 
        // current thread.
        //
        // If this method is called from a dummy guard produced by epic::unprotected(),
        // then this method is a no-op. 
        auto repin() -> void;

        // guard::repin_after()
        // Temporarily unpins the thread, executes the given function,
        // and subsequently repins the thread.
        //
        // This method is useful when you need to perform some long-running
        // operation (e.g. sleeping) and don't need to maintain any guard-based
        // reference across the call. The thread will only be repinned if this
        // is the only active guard for the current thread.
        //
        // If this method is called from a dummy guard produced by epic::unprotected(),
        // this the passed function is called directly without unpinning the thread.
        template <typename R>
        auto repin_after(std::function<R()>&& f) -> R;

        // guard::is_dummy()
        // Determines if this is a dummy `guard` created by a call to epic::unprotected(). 
        auto is_dummy() const noexcept -> bool;

        // guard::unprotected()
        // Returns a reference to a dummy guard that allows
        // unprotected access to `atomic`s.
        //
        // This guard should be used under special conditions only;
        // it does not actually keep any thread pinned - it is just
        // a fake guard that allows loading from `atomic`s unsafely.
        //
        // Calling guard::defer() on a dummy guard does not actually
        // defer the function call, it is invoked immediately.
        //
        // The most common use of this function is to produce a dummy
        // guard that is used for constructed or destructing a data structure.
        static auto unprotected() -> guard;
    };


}

#endif // EPIC_GUARD_H