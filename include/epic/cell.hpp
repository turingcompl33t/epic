// cell.hpp

#ifndef EPIC_CELL_H
#define EPIC_CELL_H

#include <utility>

namespace epic
{
    // cell
    // 
    // A mutable memory location.
    template <typename T>
    class cell
    {
        mutable T data;
    public:
        cell(T init) : data{init} {}

        cell(cell const& other) : data{other.data} {}

        cell& operator=(cell const& rhs)
        {
            this->data = rhs.data;
            return *this;
        }

        cell(cell&& other) 
            : data{std::move(other.data)} {}

        cell& operator=(cell&& rhs)
        {
            if (&rhs != this)
            {
                this->data = std::move(rhs.data);
            }

            return *this;
        }

        // cell::get()
        // Retrieve the contents of this cell.
        auto get() const noexcept -> T const
        {
            return this->data;
        }

        // cell::set()
        // Set the contents of this cell.
        auto set(T new_data) const noexcept -> void
        {
            this->data = new_data;
        }

        // cell::swap()
        // Swap the contents of this cell with the contents of another.
        auto swap(cell<T> const& other) const noexcept -> void
        {
            std::swap(this->data, other.data);
        }
    };
}

#endif // EPIC_CELL_H