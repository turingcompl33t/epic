// owned.cpp

#include <catch2/catch.hpp>

#include <epic/owned.hpp>

struct point_t
{
    size_t x;
    size_t y;

    point_t(size_t x_, size_t y_) 
        : x{x_}, y{y_} {}
};

TEST_CASE("epic::owned")
{
    using namespace epic;

    SECTION("can be constructed from initializer for primitive type")
    {
        auto o = owned<int>::create(5);
        REQUIRE(o.deref() == 5);
    }

    SECTION("can be constructed from initializer for compound type")
    {
        auto o = owned<point_t>::create(3, 4);
        auto& v = o.deref();

        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);
    }

    SECTION("can be constructed from a raw pointer")
    {
        auto* p = new point_t{3, 4};
        auto o = owned<point_t>::from_raw(p);

        auto& v = o.deref();

        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);
    }

    SECTION("can be constructed from a std::unique_ptr")
    {
        auto p = std::make_unique<point_t>(3, 4);
        auto o = owned<point_t>::from_unique(std::move(p));

        auto v = o.deref();

        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);
    }

    SECTION("can be used to construct a std::unique_ptr")
    {
        auto o = owned<point_t>::create(3, 4);

        auto v = o.deref();
        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);

        auto p = o.into_unique();

        REQUIRE(p->x == 3);
        REQUIRE(p->y == 4);
    }
}