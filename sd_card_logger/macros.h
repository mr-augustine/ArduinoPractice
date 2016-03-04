/*
  Defines useful macros for bit-twiddling, borrowed from "AVR Programming"

  Note: SFR (Spectial Function Registers) is defined in avr/sfr_defs, which
    is included by avr/io.h.
*/
#ifndef _AVR_IO_H_
#include <avr/io.h>
#endif

#define BV(bit)                 (1 << bit)
#define set_bit(sfr, bit)       (_SFR_BYTE(sfr) |= BV(bit))
#define clear_bit(sfr, bit)     (_SFR_BYTE(sfr) &= ~BV(bit))
#define toggle_bit(sfr, bit)    (_SFR_BYTE(sfr) ^= BV(bit))
#define MS_BIT                  0x80
#define LS_BIT                  0x01
