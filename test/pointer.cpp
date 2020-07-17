// pointer.cpp

#include <catch2/catch.hpp>

#include <epic/pointer.hpp>

struct point_t
{
    size_t x;
    size_t y;

    point_t(size_t x_, size_t y_)
        : x{x_}, y{y_} {}
};

TEST_CASE("epic::detail::pointer")
{
    SECTION("alignment() returns the alignment of a specified type")
    {
        using dword_t = int;
        using qword_t = unsigned long long;

        auto const d = epic::pointable<dword_t>::alignment();
        auto const q = epic::pointable<qword_t>::alignment();

        REQUIRE(d == 4);
        REQUIRE(q == 8);
    }

    SECTION("init() initializes a new pointable with trivial type")
    {
        auto const s = epic::pointable<int>::init(5);
        auto const v = epic::pointable<int>::deref(s);
        
        REQUIRE(v == 5);

        epic::pointable<int>::drop(s);
    }

    SECTION("init() initializes a new pointable with a compound type")
    {
        auto const s = epic::pointable<point_t>::init(1, 2);
        auto const v = epic::pointable<point_t>::deref(s);

        REQUIRE(v.x == 1);
        REQUIRE(v.y == 2);

        epic::pointable<point_t>::drop(s);
    }
}