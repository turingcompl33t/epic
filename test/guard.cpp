// test/guard.cpp

#include <catch2/catch.hpp>
#include <epic/guard.hpp>

TEST_CASE("epic::guard")
{
    SECTION("may be default constructed to produce a dummy guard")
    {
        auto g = epic::guard{};
        REQUIRE(g.is_dummy());
    }
}