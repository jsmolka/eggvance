#pragma once

#include "registerbank.h"

class Registers : public RegisterBank
{
public:
    void reset();

    bool arm() const;
    bool thumb() const;
    void setThumb(bool set);

    int z() const;
    int n() const;
    int c() const;
    int v() const;

    void setZ(bool set);
    void setN(bool set);
    void setC(bool set);
    void setV(bool set);

    bool check(Condition condition) const;

    u32  operator[](int index) const;
    u32& operator[](int index);

private:
    void setFlag(CPSR flag, bool set);
};

