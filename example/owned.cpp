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
};

int main()
{
    using namespace epic;

    auto* p = new point_t{3, 4};
    auto o = owned<point_t>::from_raw(p);

    return SUCCESS;
}