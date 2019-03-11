#pragma once

#include "registerbank.h"

class Registers : public RegisterBank
{
public:
    void reset();

    bool isThumb() const;

    u8 z() const;
    u8 n() const;
    u8 c() const;
    u8 v() const;

    void setZ(bool set);
    void setN(bool set);
    void setC(bool set);
    void setV(bool set);

    bool checkCondition(Condition cond) const;

    u32 operator[](int index) const;
    u32& operator[](int index);

private:
    void setFlag(CPSR flag, bool set);
};

