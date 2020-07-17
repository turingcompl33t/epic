// shared.cpp

#include <catch2/catch.hpp>

#include <epic/shared.hpp>

TEST_CASE("epic::shared")
{
    using namespace epic;

    SECTION("can be constructed with null pointer value with share::null()")
    {
        auto s = shared<int>::null();
        REQUIRE(s.is_null());
    }

    SECTION("can be constructed from raw pointer")
    {
        auto* ptr = new int{17};
        auto s = shared<int>::from_raw(ptr);

        REQUIRE(*s == 17);
        delete s.as_raw();
    }

    SECTION("method clone() creates a new shared<T> instance that references same pointee")
    {   
        auto* ptr = new int{17};

        auto s = shared<int>::from_raw(ptr);
        auto c = s.clone();

        REQUIRE(*s == 17);
        REQUIRE(*c == 17);

        *s = 1054;

        REQUIRE(*s == 1054);
        REQUIRE(*c == 1054);

        delete c.as_raw();
    }
}