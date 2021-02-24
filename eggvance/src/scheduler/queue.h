#pragma once

#include <queue>

template<typename T>
class Queue : public std::priority_queue<T, std::vector<T>, std::greater<T>>
{
public:
    void remove(const T& value)
    {
        auto iter = std::find(this->c.begin(), this->c.end(), value);

        if (iter != this->c.end())
        {
            this->c.erase(iter);
            std::make_heap(this->c.begin(), this->c.end(), this->comp);
        }
    }
};
