// test/nullable_ref.cpp

#include <catch2/catch.hpp>

#include <memory>
#include <epic/nullable_ref.hpp>

using namespace epic;

TEST_CASE("epic::nullable_ref supports default construction to an invalid reference")
{
    SECTION("that supports an explicit boolean operator")
    {
        auto r = nullable_ref<int>{};
        REQUIRE_FALSE(static_cast<bool>(r));
    }

    SECTION("that throws on attempt to dereference")
    {
        auto r = nullable_ref<int>{};
        REQUIRE_THROWS_AS(*r, std::runtime_error);
    }
}

TEST_CASE("epic::nullable_ref supports construction from raw pointer into refernce type")
{
    SECTION("that supports an explicit boolean operator")
    {
        auto p = std::make_unique<int>(5);
        auto r = nullable_ref<int>{p.get()};

        REQUIRE(static_cast<bool>(r));
    }

    SECTION("that does not throw on dereference, and yields refernce to wrapped object")
    {
        auto p = std::make_unique<int>(5);
        auto r = nullable_ref<int>(p.get());

        REQUIRE_NOTHROW(*r);

        REQUIRE(*r == 5);
    }
}