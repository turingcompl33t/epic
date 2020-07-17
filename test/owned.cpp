// owned.cpp

#include <catch2/catch.hpp>

#include <epic/owned.hpp>

struct point_t
{
    size_t x;
    size_t y;

    point_t(size_t x_, size_t y_) 
        : x{x_}, y{y_} {}

    point_t(point_t const& p)
        : x{p.x}, y{p.y} {}
};

TEST_CASE("epic::owned")
{
    using namespace epic;

    SECTION("can be constructed from initializer for primitive type")
    {
        auto o = owned<int>::make(5);
        REQUIRE(*o == 5);
    }

    SECTION("can be constructed from initializer for compound type")
    {
        auto o = owned<point_t>::make(3, 4);
        
        auto const& v = *o;
        REQUIRE(v.x == 3);
        REQUIRE(v.y == 4);

        REQUIRE(o->x == 3);
        REQUIRE(o->y == 4);
    }

    SECTION("can be constructed from a raw pointer")
    {
        auto* p = new point_t{3, 4};
        auto o = owned<point_t>::from_raw(p);

        REQUIRE(o->x == 3);
        REQUIRE(o->y == 4);
    }

    SECTION("can be constructed from a std::unique_ptr")
    {
        auto p = std::make_unique<point_t>(3, 4);
        auto o = owned<point_t>::from_unique(std::move(p));

        REQUIRE(o->x == 3);
        REQUIRE(o->y == 4);
    }

    SECTION("can be used to construct a std::unique_ptr")
    {
        auto o = owned<point_t>::make(3, 4);

        REQUIRE(o->x == 3);
        REQUIRE(o->y == 4);

        auto p = o.into_unique();

        REQUIRE(p->x == 3);
        REQUIRE(p->y == 4);
    }

    SECTION("can be constructed with the standalone factory epic::make_owned()")
    {
        auto o = make_owned<point_t>(3, 4);
        
        REQUIRE(o->x == 3);
        REQUIRE(o->y == 4);
    }

    SECTION("can be cloned to produce a new deep copy of owned instance")
    {
        // TODO: why does this behave strangely when run under Catch?

        auto o = epic::make_owned<point_t>(3, 4);
        auto c = o.clone();

        REQUIRE(c->x == 3);
        REQUIRE(c->y == 4);    
    }
}