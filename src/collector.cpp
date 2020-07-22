// collector.cpp

#include <epic/collector.hpp>

#include <epic/local_handle.hpp>
#include <epic/global.hpp>
#include <epic/local.hpp>

namespace epic
{
    collector::collector() 
        : instance{std::move(std::make_shared<global>())} {}

    collector::collector(collector const& c) 
        : instance{c.instance} {}

    collector& collector::operator=(collector const& c)
    {
        instance = c.instance;
        return *this;
    }

    collector::collector(collector&& c) 
        : instance{std::move(c.instance)}
    {}

    collector& collector::operator=(collector&& c)
    {
        if (&c != this)
        {
            instance = std::move(c.instance);
        }

        return *this;
    }

    auto collector::register_handle() -> local_handle
    {
        return local::register_handle(*this);
    }

    auto collector::release() -> void
    {
        instance.reset();
    }
}