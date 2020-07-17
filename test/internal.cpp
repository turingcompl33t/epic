// test/internal.cpp

#include <catch2/catch.hpp>

#include <epic/internal.hpp>

TEST_CASE("epic::bag")
{
    using namespace epic;

    SECTION("supports default construction with bag::make()")
    {
        auto b = bag::make();
        REQUIRE(b->is_empty());
    }

    SECTION("method try_push() returns optional deferred on failed push")
    {
        unsigned long x{};

        auto b = bag::make();

        auto r1 = b->try_push(deferred::make([&]() mutable { ++x; }));
        auto r2 = b->try_push(deferred::make([&]() mutable { ++x; }));
        auto r3 = b->try_push(deferred::make([&]() mutable { ++x; }));
        auto r4 = b->try_push(deferred::make([&]() mutable { ++x; }));

        REQUIRE_FALSE(r1.has_value());
        REQUIRE_FALSE(r2.has_value());
        REQUIRE_FALSE(r3.has_value());
        REQUIRE_FALSE(r4.has_value());

        // fifth push operation fails because bag is full
        auto r5 = b->try_push(deferred::make([&]() mutable { ++x; }));

        // the return value from try_push() is an optional containing 
        // the deferred function that could not be pushed into the bag 
        REQUIRE(r5.has_value());
        
        // none of the functions pushed into the bag have run
        REQUIRE(x == 0);

        // invoke the deferred function we got back as result of failed try_push()
        r5.value().call();

        // the deferred function is invoked successfully
        REQUIRE(x == 1);
    }
}