// atomic.cpp

#include <catch2/catch.hpp>

#include <atomic>

#include <epic/guard.hpp>
#include <epic/atomic.hpp>

TEST_CASE("epic::atomic")
{
    using namespace epic;

    SECTION("can be constructed via atomic::null()")
    {
        auto a = atomic<int>::null();
        REQUIRE(a.is_null());
    }

    SECTION("can be constructed via make() static factory")
    {
        auto a = atomic<int>::make(5);
        auto g = guard{};
        auto s = a.load(std::memory_order_acquire, g);
        REQUIRE(*s == 5);
    }

    SECTION("can be constructed via epic::make_atomic() factory")
    {
        auto a = make_atomic<int>(5);
        auto g = guard{};
        auto s = a.load(std::memory_order_acquire, g);
        REQUIRE(*s == 5);
    }

    SECTION("supports atomic store operations")
    {
        auto a = make_atomic<int>(5);
        auto g = guard{};

        auto first = a.load(std::memory_order_acquire, g);
        REQUIRE(*first == 5);

        auto o = make_owned<int>(17);
        a.store(std::move(o), std::memory_order_release);

        auto second = a.load(std::memory_order_acquire, g);
        REQUIRE(*second == 17);
    }
}