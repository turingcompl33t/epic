// atomic.cpp

#include <epic/guard.hpp>
#include <epic/atomic.hpp>

constexpr static auto const SUCCESS = 0x0;
constexpr static auto const FAILURE = 0x1;

int main()
{
    using namespace epic;

    auto a = atomic<int>::make(5);
    auto g = guard{};
    auto s = a.load(std::memory_order_acquire, g);

    printf("loaded = %d\n", *s);

    return SUCCESS;
}