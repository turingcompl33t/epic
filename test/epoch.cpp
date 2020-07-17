// test/epoch.cpp

#include <catch2/catch.hpp>

#include <epic/epoch.hpp>

TEST_CASE("epic::epoch")
{
    using namespace epic;

    SECTION("can be initialized to the starting epoch with starting()")
    {
        auto e = epoch::starting();
        REQUIRE(e.get() == 0);
    }

    SECTION("may be initialized with an arbitrary value with with_value()")
    {
        auto e = epoch::with_value(5);
        REQUIRE(e.get() == 5);
    }

    SECTION("supports pinned() and unpinned() operations")
    {
        auto e1 = epoch::starting();
        REQUIRE_FALSE(epoch::is_pinned(std::move(e1)));

        auto e2 = epoch::starting();
        auto p = epoch::pinned(std::move(e2));
        REQUIRE(epoch::is_pinned(std::move(p)));

        auto e3 = epoch::starting();
        auto u = epoch::unpinned(std::move(e3));
        REQUIRE_FALSE(epoch::is_pinned(std::move(u)));
    }

    SECTION("has wrapping addition / subtraction semantics")
    {
        auto e1 = epoch::with_value(USIZE_MAX - 1);
        auto e2 = epoch::successor(std::move(e1));

        REQUIRE(e2.get() == 0);

        auto e3 = epoch::with_value(2);
        auto e4 = epoch::starting();

        // compute the number of epochs e3 is ahead of e4
        auto r = epoch::wrapping_sub(std::move(e3), std::move(e4));
        
        // recall: successor epoch is 2 ahead of the current epoch
        // in terms of the internal representation
        REQUIRE(r == 1);
    }
}

TEST_CASE("epic::atomic_epoch")
{
    using namespace epic;

    SECTION("can be created from an existing epoch")
    {
        auto e = epoch::starting();
        auto a = atomic_epoch::make(std::move(e));

        auto l = a.load(std::memory_order_acquire);
        REQUIRE(l.get() == 0);
    }

    SECTION("supports store() operations from an existing epoch")
    {
        auto e = epoch::starting();
        auto a = atomic_epoch::make(std::move(e));

        auto s = epoch::with_value(128);
        a.store(std::move(s), std::memory_order_release);

        auto l = a.load(std::memory_order_acquire);
        REQUIRE(l.get() == 128);
    }
}