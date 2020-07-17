// shared.cpp

#include <epic/shared.hpp>

constexpr static auto const SUCCESS = 0x0;
constexpr static auto const FAILURE = 0x1;

int main()
{
    using namespace epic;

    auto s = shared<int>::null();

    return SUCCESS;
}