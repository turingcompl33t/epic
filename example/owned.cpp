// example/owned.cpp

#include <cstdio>

#include <epic/owned.hpp>

constexpr static auto const SUCCESS = 0x0;
constexpr static auto const FAILURE = 0x1;

struct point_t
{
    size_t x;
    size_t y;

    point_t(size_t x_, size_t y_) 
        : x{x_}, y{y_} {}

    point_t(point_t const& p)
        : x{p.x}, y{p.y} {}
};

int main()
{
    using namespace epic;

    auto o = make_owned<point_t>(3, 4);
    auto c = o.clone();

    o->x = 5;
    o->y = 6;

    printf("cloned x = %zu\n", c->x);
    printf("cloned y = %zu\n", c->y);

    printf("original x = %zu\n", o->x);
    printf("original y = %zu\n", o->y);

    return SUCCESS;
}