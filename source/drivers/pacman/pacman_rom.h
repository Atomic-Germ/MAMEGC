/***************************************************************************
 * Pac-Man Test ROM
 * 
 * Simple test program to verify Pac-Man hardware
 ***************************************************************************/

#ifndef PACMAN_ROM_H
#define PACMAN_ROM_H

#include "../../mame2003/osd_gc.h"

/***************************************************************************
 * Test ROM: Fill video RAM with test pattern
 * 
 * This program:
 * 1. Fills video RAM with a checkerboard pattern
 * 2. Fills color RAM with colors
 * 3. Loops forever
 ***************************************************************************/

static const UINT8 pacman_test_rom[] = {
    /* 0000: Initialize */
    0x31, 0x00, 0x50,        /* LD SP, 0x5000    ; Set stack pointer */
    
    /* 0003: Fill Video RAM with test pattern */
    0x21, 0x00, 0x40,        /* LD HL, 0x4000    ; Point to VRAM start */
    0x06, 0x04,              /* LD B, 0x04       ; 4 pages (1024 bytes) */
    0x0E, 0x00,              /* LD C, 0x00       ; Counter */
    
    /* 000B: VRAM_LOOP */
    0x71,                    /* LD (HL), C       ; Write counter to VRAM */
    0x23,                    /* INC HL           ; Next address */
    0x0C,                    /* INC C            ; Next value */
    0x7C,                    /* LD A, H          ; Check if done */
    0xFE, 0x44,              /* CP 0x44          ; End of VRAM? */
    0x20, 0xF7,              /* JR NZ, VRAM_LOOP ; Loop if not done */
    
    /* 0014: Fill Color RAM with colors */
    0x21, 0x00, 0x44,        /* LD HL, 0x4400    ; Point to CRAM start */
    0x06, 0x04,              /* LD B, 0x04       ; 4 pages */
    0x0E, 0x0F,              /* LD C, 0x0F       ; Color value */
    
    /* 001C: CRAM_LOOP */
    0x71,                    /* LD (HL), C       ; Write color to CRAM */
    0x23,                    /* INC HL           ; Next address */
    0x7C,                    /* LD A, H          ; Check if done */
    0xFE, 0x48,              /* CP 0x48          ; End of CRAM? */
    0x20, 0xF9,              /* JR NZ, CRAM_LOOP ; Loop if not done */
    
    /* 0024: Enable interrupts and loop */
    0x3E, 0x01,              /* LD A, 0x01       ; Interrupt enable */
    0x32, 0x00, 0x50,        /* LD (0x5000), A   ; Write to interrupt enable */
    
    /* 002A: MAIN_LOOP */
    0x76,                    /* HALT             ; Wait for interrupt */
    0x18, 0xFD,              /* JR MAIN_LOOP     ; Loop forever */
};

#define PACMAN_TEST_ROM_SIZE sizeof(pacman_test_rom)

#endif /* PACMAN_ROM_H */
