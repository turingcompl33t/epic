// test/bag.cpp

#include <catch2/catch.hpp>
#include <epic/bag.hpp>
#include <epic/epoch.hpp>

#include <memory>

TEST_CASE("epic::bag")
{
    using namespace epic;

    SECTION("supports default construction with a bunch of no-op functions")
    {
        auto b = std::make_unique<bag>();
        REQUIRE(b->is_empty());
    }

    SECTION("invokes all stored deferred functions on destruction")
    {
        unsigned long x{};

        auto* b = new bag{};

        auto r1 = b->try_push(deferred{([&]() mutable { ++x; })});
        auto r2 = b->try_push(deferred{([&]() mutable { ++x; })});

        REQUIRE_FALSE(r1.has_value());
        REQUIRE_FALSE(r2.has_value());

        // none of the deferred functions have run
        REQUIRE(x == 0);

        // destroy the bag
        delete b;

        // now the stored functions have run
        REQUIRE(x == 2);
    }

    SECTION("method try_push() returns optional deferred on failed push")
    {
        unsigned long x{};

        auto b = std::make_unique<bag>();

        auto r1 = b->try_push(deferred{([&]() mutable { ++x; })});
        auto r2 = b->try_push(deferred{([&]() mutable { ++x; })});
        auto r3 = b->try_push(deferred{([&]() mutable { ++x; })});
        auto r4 = b->try_push(deferred{([&]() mutable { ++x; })});

        REQUIRE_FALSE(r1.has_value());
        REQUIRE_FALSE(r2.has_value());
        REQUIRE_FALSE(r3.has_value());
        REQUIRE_FALSE(r4.has_value());

        // fifth push operation fails because bag is full
        auto r5 = b->try_push(deferred{([&]() mutable { ++x; })});

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

    SECTION("method try_push() throws when invoked on a sealed bag")
    {
        unsigned long x{};

        auto b = std::make_unique<bag>();

        b->try_push(deferred{[&x](){ ++x; }});
        b->try_push(deferred{[&x](){ ++x; }});

        auto e = epoch::with_value(16);

        b->seal(e);

        REQUIRE_THROWS_AS(b->try_push(deferred{[&x](){ ++x; }}), std::runtime_error);
    }
}