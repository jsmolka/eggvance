#pragma once

template<typename T>
class IteratorRange
{
public:
    using iterator = T;
    using const_iterator = const T;

    IteratorRange(iterator begin, iterator end)
        : begin_(begin), end_(end) {}

    iterator begin() { return begin_; }
    iterator end() { return end_; }
    const_iterator begin() const { return begin_; }
    const_iterator end() const { return end_; }
    const_iterator cbegin() const { return begin_; }
    const_iterator cend() const { return end_; }

private:
    const_iterator begin_;
    const_iterator end_;
};
