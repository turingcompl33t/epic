// test/deferred.cpp

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <epic/deferred.hpp>

TEST_CASE("epic::deferred")
{
    using namespace epic;

    SECTION("supports construction from lambda with captures")
    {
        unsigned long x{};

        deferred d{[&x](){ ++x; }};

        REQUIRE(x == 0);

        d.call();

        REQUIRE(x == 1);
    }

    SECTION("supports swap with other deferred function wrappers")
    {
        unsigned long x{};
        unsigned long y{};

        deferred x_incrementer{[&x](){ ++x; }};
        deferred y_incrementer{[&y](){ ++y; }};

        REQUIRE(x == 0);
        REQUIRE(y == 0);

        x_incrementer.swap(y_incrementer);

        x_incrementer.call();
        REQUIRE(x == 0);
        REQUIRE(y == 1);

        y_incrementer.call();
        REQUIRE(x == 1);
        REQUIRE(y == 1);
    }
}