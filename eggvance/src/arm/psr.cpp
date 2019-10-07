#include "psr.h"

#include "common/macros.h"

PSR& PSR::operator=(u32 value)
{
    packed = value;
    return *this;
}

PSR::operator u32() const
{
    return packed;
}

bool PSR::matches(Condition condition) const
{
    switch (condition)
    {
    case Condition::EQ: return z;
    case Condition::NE: return !z;
    case Condition::CS: return c;
    case Condition::CC: return !c;
    case Condition::MI: return n;
    case Condition::PL: return !n;
    case Condition::VS: return v;
    case Condition::VC: return !v;
    case Condition::HI: return c && !z;
    case Condition::LS: return !c || z;
    case Condition::GE: return n == v;
    case Condition::LT: return n != v;
    case Condition::GT: return !z && (n == v);
    case Condition::LE: return z || (n != v);
    case Condition::AL: return true;
    case Condition::NV: return false;

    default:
        EGG_UNREACHABLE;
        return false;
    }
}