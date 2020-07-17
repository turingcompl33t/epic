// deferred.cpp

#include <cstdio>

#include <epic/deferred.hpp>

constexpr static auto const SUCCESS = 0x0;
constexpr static auto const FAILURE = 0x1;

int main()
{
    using namespace epic;

    unsigned long x{};

    auto d = deferred::make(
        [&]() mutable { x = 25; });
    
    d.call();

    printf("x = %lu\n", x);

    return SUCCESS;
}