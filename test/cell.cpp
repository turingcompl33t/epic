// cell.cpp

#include <catch2/catch.hpp>

#include <epic/cell.hpp>

class dummy
{
    epic::cell<int> a;

public:
    dummy() : a{0} {}

    dummy(dummy const&)            = delete;
    dummy& operator=(dummy const&) = delete;

    dummy(dummy&&)            = delete;
    dummy& operator=(dummy&&) = delete;

    auto get() const noexcept -> int
    {
        return a.get();
    }

    auto set(int val) const noexcept -> void
    {
        a.set(val);
    }

    auto swap(dummy const& other) const noexcept -> void
    {
        this->a.swap(other.a);
    }
};

TEST_CASE("epic::cell enables interior mutability")
{
    dummy d1{};
    dummy d2{};

    REQUIRE(d1.get() == 0);
    REQUIRE(d2.get() == 0);

    d1.set(1776);
    d2.set(1054);

    REQUIRE(d1.get() == 1776);
    REQUIRE(d2.get() == 1054);

    d1.swap(d2);

    REQUIRE(d1.get() == 1054);
    REQUIRE(d2.get() == 1776);
}