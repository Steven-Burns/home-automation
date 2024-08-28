#pragma once

#if defined AVR

// per https://forum.arduino.cc/t/soft-reset-and-arduino/367284/5

#include <avr/wdt.h>

inline void reboot()
{
    wdt_disable();
    wdt_enable(WDTO_15MS);
    while (1)
    {
    }
}

#elif defined(TARGET_RP2040) 

#include <hardware/regs/m0plus.h>
#include <hardware/regs/addressmap.h>
#include <hardware/resets.h>

inline void reboot()
{
    // reset USB controller
    reset_block(RESETS_WDSEL_USBCTRL_BITS);
    // you might want to add other perpherials here if you use them

    // reset the CPU
    auto &AIRCR_register = *(volatile uint32_t *)(PPB_BASE + M0PLUS_AIRCR_OFFSET);
    // From datasheet:
    // 31:16 VECTKEY: On writes, write 0x05FA to VECTKEY, otherwise the write is ignored.
    // 15 ENDIANESS: 0 = Little-endian.
    // 14:3 Reserved
    // 2 SYSRESETREQ: Writing 1 to this bit causes the SYSRESETREQ signal to the outer system to be asserted to request a reset.
    // 1 VECTCLRACTIVE: not relevant here
    AIRCR_register = (0x05FA << M0PLUS_AIRCR_VECTKEY_LSB) | M0PLUS_AIRCR_SYSRESETREQ_BITS;
}

#endif