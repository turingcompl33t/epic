// scope_guard.cpp

#include <catch2/catch.hpp>
#include <epic/scope_guard.hpp>

#include <stdexcept>

static unsigned long global_x{0};

static auto i_throw() -> void
{
    epic::scope_guard sg{[](){ ++global_x; }};
    throw std::runtime_error{"I told you I throw"};
}

TEST_CASE("epic::scope_guard")
{
    SECTION("invokes the provided function on scope exit")
    {
        unsigned long x{};

        {
            epic::scope_guard sg{[&x](){ ++x; }};
        }

        REQUIRE(x == 1);
    }

    SECTION("invokes the provided function on scope exit from throwing function")
    {
        REQUIRE(global_x == 0);

        try
        {
            i_throw();
        }
        catch (std::runtime_error const&) {}

        REQUIRE(global_x == 1);
    }
}