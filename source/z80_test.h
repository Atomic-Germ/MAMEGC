/***************************************************************************
 * Z80 Test Program
 * 
 * Simple test program to verify Z80 execution
 ***************************************************************************/

#ifndef Z80_TEST_H
#define Z80_TEST_H

#include "mame2003/osd_gc.h"

/***************************************************************************
 * Test Program 1: Simple Loop
 * 
 * This program:
 * 1. Loads A with 0x42
 * 2. Loads B with 0x10
 * 3. Loops decrementing B
 * 4. Halts
 ***************************************************************************/

static const UINT8 z80_test_program[] = {
    /* 0000 */ 0x3E, 0x42,        /* LD A, 0x42    ; Load A with 0x42 */
    /* 0002 */ 0x06, 0x10,        /* LD B, 0x10    ; Load B with 0x10 */
    /* 0004 */ 0x05,              /* LOOP: DEC B   ; Decrement B */
    /* 0005 */ 0x20, 0xFD,        /* JR NZ, LOOP   ; Jump if not zero */
    /* 0007 */ 0x76,              /* HALT          ; Halt CPU */
};

#define Z80_TEST_PROGRAM_SIZE sizeof(z80_test_program)

/***************************************************************************
 * Test Program 2: Memory Write Test
 * 
 * This program:
 * 1. Writes 0xAA to address 0x4000
 * 2. Reads it back
 * 3. Writes 0x55 to address 0x4001
 * 4. Halts
 ***************************************************************************/

static const UINT8 z80_memtest_program[] = {
    /* 0000 */ 0x21, 0x00, 0x40,  /* LD HL, 0x4000 ; Point to RAM */
    /* 0003 */ 0x3E, 0xAA,        /* LD A, 0xAA    ; Load pattern */
    /* 0005 */ 0x77,              /* LD (HL), A    ; Write to RAM */
    /* 0006 */ 0x23,              /* INC HL        ; Next address */
    /* 0007 */ 0x3E, 0x55,        /* LD A, 0x55    ; Load pattern */
    /* 0009 */ 0x77,              /* LD (HL), A    ; Write to RAM */
    /* 000A */ 0x76,              /* HALT          ; Halt CPU */
};

#define Z80_MEMTEST_PROGRAM_SIZE sizeof(z80_memtest_program)

/***************************************************************************
 * Test Program 3: Stack Test
 * 
 * This program:
 * 1. Sets stack pointer to 0x5000
 * 2. Pushes 0x1234 onto stack
 * 3. Pops it back
 * 4. Halts
 ***************************************************************************/

static const UINT8 z80_stacktest_program[] = {
    /* 0000 */ 0x31, 0x00, 0x50,  /* LD SP, 0x5000 ; Set stack */
    /* 0003 */ 0x01, 0x34, 0x12,  /* LD BC, 0x1234 ; Load value */
    /* 0006 */ 0xC5,              /* PUSH BC       ; Push to stack */
    /* 0007 */ 0xD1,              /* POP DE        ; Pop to DE */
    /* 0008 */ 0x76,              /* HALT          ; Halt CPU */
};

#define Z80_STACKTEST_PROGRAM_SIZE sizeof(z80_stacktest_program)

#endif /* Z80_TEST_H */
