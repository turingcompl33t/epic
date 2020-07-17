// ordering.cpp

#include <catch2/catch.hpp>

#include <atomic>
#include <epic/ordering.hpp>

TEST_CASE("epic::strongest_failure_ordering()")
{
    SECTION("returns correct ordering for std::memory_order_relaxed")
    {   
        auto const r = epic::strongest_failure_ordering(std::memory_order_relaxed);
        REQUIRE(r == std::memory_order_relaxed);
    }

    SECTION("returns correct ordering for std::memory_order_release")
    {
        auto const r = epic::strongest_failure_ordering(std::memory_order_release);
        REQUIRE(r == std::memory_order_relaxed);
    }

    SECTION("returns correct ordering for std:memory_order_acquire")
    {
        auto const r = epic::strongest_failure_ordering(std::memory_order_acquire);
        REQUIRE(r == std::memory_order_acquire);
    }

    SECTION("returns correct ordering for std:memory_order_acq_rel")
    {
        auto const r = epic::strongest_failure_ordering(std::memory_order_acq_rel);
        REQUIRE(r == std::memory_order_acquire);
    }

    SECTION("returns correct ordering for std:memory_order_seq_cst")
    {
        auto const r = epic::strongest_failure_ordering(std::memory_order_seq_cst);
        REQUIRE(r == std::memory_order_seq_cst);
    }
}

TEST_CASE("epic::ordering_success()")
{
    SECTION("returns correct value in (std::memory_order) overload")
    {
        auto const r = epic::ordering_success(std::memory_order_seq_cst);
        REQUIRE(r == std::memory_order_seq_cst);
    }

    SECTION("returns the correct value in (std::pair<> overload")
    {
        auto const p = std::make_pair(std::memory_order_acquire, std::memory_order_release);
        auto const r = epic::ordering_success(p);
        REQUIRE(r == std::memory_order_acquire);
    }
}

TEST_CASE("epic::ordering_failure()")
{
    SECTION("returns the correct value in (std::memory_order) overload")
    {
        auto const r = epic::ordering_failure(std::memory_order_relaxed);
        REQUIRE(r == std::memory_order_relaxed);
    }

    SECTION("returns the correct value in (std::pair<>) overload")
    {
        auto const p = std::make_pair(std::memory_order_acquire, std::memory_order_release);
        auto const r = epic::ordering_failure(p);
        REQUIRE(r == std::memory_order_release);
    }
}