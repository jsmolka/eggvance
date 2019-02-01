#include "arm7.h"

#include <iostream>

ARM7::ARM7()
{

}

void ARM7::reset()
{
    mode = MODE_USR;
}

u32 ARM7::reg(u8 number) const
{
    switch (number)
    {
    case 0: return regs.r0;
    case 1: return regs.r1;
    case 2: return regs.r2;
    case 3: return regs.r3;
    case 4: return regs.r4;
    case 5: return regs.r5;
    case 6: return regs.r6;
    case 7: return regs.r7;
    case 8: return mode == MODE_FIQ ? regs.r8_fiq : regs.r8;
    case 9: return mode == MODE_FIQ ? regs.r9_fiq : regs.r9;
    case 10: return mode == MODE_FIQ ? regs.r10_fiq : regs.r10;
    case 11: return mode == MODE_FIQ ? regs.r11_fiq : regs.r11;
    case 12: return mode == MODE_FIQ ? regs.r12_fiq : regs.r12;

    case 13:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: return regs.r13;
        case MODE_FIQ: return regs.r13_fiq;
        case MODE_SVC: return regs.r13_svc;
        case MODE_ABT: return regs.r13_abt;
        case MODE_IRQ: return regs.r13_irq;
        case MODE_UND: return regs.r13_und;
        }

    case 14:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: return regs.r14;
        case MODE_FIQ: return regs.r14_fiq;
        case MODE_SVC: return regs.r14_svc;
        case MODE_ABT: return regs.r14_abt;
        case MODE_IRQ: return regs.r14_irq;
        case MODE_UND: return regs.r14_und;
        }

    case 15: return regs.r15;

    default:
        std::cout << __FUNCTION__ << " - Tried accessing invalid register " << (int)number << "\n";
    }
    return 0;
}

void ARM7::setReg(u8 number, u32 value)
{
    switch (number)
    {
    case 0: regs.r0 = value; break;
    case 1: regs.r1 = value; break;
    case 2: regs.r2 = value; break;
    case 3: regs.r3 = value; break;
    case 4: regs.r4 = value; break;
    case 5: regs.r5 = value; break;
    case 6: regs.r6 = value; break;
    case 7: regs.r7 = value; break;
    case 8: 
        if (mode == MODE_FIQ)
            regs.r8_fiq = value;
        else
            regs.r8 = value;
        break;    
    
    case 9: 
        if (mode == MODE_FIQ)
            regs.r9_fiq = value;
        else
            regs.r9 = value;
        break;    

    case 10: 
        if (mode == MODE_FIQ)
            regs.r10_fiq = value;
        else
            regs.r10 = value;
        break;    

    case 11: 
        if (mode == MODE_FIQ)
            regs.r11_fiq = value;
        else
            regs.r11 = value;
        break;    

    case 12: 
        if (mode == MODE_FIQ)
            regs.r12_fiq = value;
        else
            regs.r12 = value;
        break;

    case 13:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: regs.r13 = value; break;
        case MODE_FIQ: regs.r13_fiq = value; break;
        case MODE_SVC: regs.r13_svc = value; break;
        case MODE_ABT: regs.r13_abt = value; break;
        case MODE_IRQ: regs.r13_irq = value; break;
        case MODE_UND: regs.r13_und = value; break;
        }
        break;

    case 14:
        switch (mode)
        {
        case MODE_USR:
        case MODE_SYS: regs.r14 = value; break;
        case MODE_FIQ: regs.r14_fiq = value; break;
        case MODE_SVC: regs.r14_svc = value; break;
        case MODE_ABT: regs.r14_abt = value; break;
        case MODE_IRQ: regs.r14_irq = value; break;
        case MODE_UND: regs.r14_und = value; break;
        }
        break;

    case 15: regs.r15 = value; break;

    default:
        std::cout << __FUNCTION__ << " - Tried setting invalid register " << (int)number << "\n";
    }
}

u32 ARM7::spsr(u8 number) const
{
    switch (mode)
    {
    case MODE_FIQ: return regs.spsr_fiq;
    case MODE_SVC: return regs.spsr_svc;
    case MODE_ABT: return regs.spsr_abt;
    case MODE_IRQ: return regs.spsr_fiq;
    case MODE_UND: return regs.spsr_und;

    default:
        std::cout << __FUNCTION__ << " - Tried accessing invalid spsr " << (int)number << "\n";
    }
    return 0;
}

void ARM7::setSpsr(u8 number, u32 value)
{
    switch (mode)
    {
    case MODE_FIQ: regs.spsr_fiq = value; break;
    case MODE_SVC: regs.spsr_svc = value; break;
    case MODE_ABT: regs.spsr_abt = value; break;
    case MODE_IRQ: regs.spsr_fiq = value; break;
    case MODE_UND: regs.spsr_und = value; break;

    default:
        std::cout << __FUNCTION__ << " - Tried setting invalid spsr " << (int)number << "\n";
    }
}
