#include "event.h"

bool Event::operator<(const Event& other) const
{
    return when < other.when;
}

bool Event::operator>(const Event& other) const
{
    return when > other.when;
}

bool Event::isScheduled() const
{
    return when;
}
