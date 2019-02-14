#pragma once

class Internal
{
public:
    Internal() = default;
    Internal(const Internal&) = delete;
    virtual ~Internal() = default;

    Internal& operator=(const Internal&) = delete;

    virtual void reset() = 0;
};