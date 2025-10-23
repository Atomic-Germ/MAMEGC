/***************************************************************************
 * GameCube OSD CPU Definitions
 * 
 * This file defines CPU-specific types and macros for MAME2003
 ***************************************************************************/

#ifndef OSD_CPU_H
#define OSD_CPU_H

#include "osd_gc.h"

/***************************************************************************
 * CPU Interface Numbers
 * These identify different CPU types that MAME can emulate
 ***************************************************************************/

#define CPU_DUMMY       0
#define CPU_Z80         1
#define CPU_M6502       2
#define CPU_M6809       3
#define CPU_M68000      4
#define CPU_I8085       5
#define CPU_I8086       6
#define CPU_COUNT       128  /* Maximum CPUs */

/***************************************************************************
 * CPU Flags
 ***************************************************************************/

#define CPU_16BIT_PORT  0x00000001  /* CPU has 16-bit ports */
#define CPU_16BIT_MEMORY 0x00000002 /* CPU has 16-bit memory */
#define CPU_AUDIO_CPU   0x00000004  /* This is an audio CPU */

/***************************************************************************
 * CPU Context Size
 * Each CPU core needs a context to store register state
 ***************************************************************************/

#ifndef MAX_CPU
#define MAX_CPU 8  /* Maximum number of CPUs in a driver */
#endif

/* CPU context (opaque to drivers) */
typedef struct {
    UINT8 context[256];  /* Enough for most CPU contexts */
} cpu_context;

/***************************************************************************
 * CPU Interface Structure
 * Each CPU core provides this structure
 ***************************************************************************/

typedef struct {
    /* CPU identification */
    unsigned int cpu_type;
    unsigned int num_irqs;
    int default_vector;
    
    /* Function pointers (will be implemented per-CPU) */
    void (*reset)(void *param);
    void (*exit)(void);
    int  (*execute)(int cycles);
    void (*burn)(int cycles);
    unsigned (*get_context)(void *dst);
    void (*set_context)(void *src);
    unsigned (*get_pc)(void);
    void (*set_pc)(unsigned val);
    unsigned (*get_sp)(void);
    void (*set_sp)(unsigned val);
    unsigned (*get_reg)(int regnum);
    void (*set_reg)(int regnum, unsigned val);
    void (*set_nmi_line)(int state);
    void (*set_irq_line)(int irqline, int state);
    void (*set_irq_callback)(int (*callback)(int irqline));
    const char *(*info)(void *context, int regnum);
    unsigned (*dasm)(char *buffer, unsigned pc);
    
    /* Register names */
    const char **reg_names;
    
    /* Flags */
    unsigned int flags;
    
    /* Address bus widths */
    int address_shift;
    int address_bits;
    int endianess;
    int align_unit;
    int max_inst_len;
    
    /* Cycle times */
    int overclock;
} cpu_interface;

/***************************************************************************
 * CPU Endianness
 ***************************************************************************/

#define CPU_IS_LE       0  /* Little endian */
#define CPU_IS_BE       1  /* Big endian */

/***************************************************************************
 * CPU Register IDs (common to all CPUs)
 ***************************************************************************/

#define MAX_REGS        128

/* Generic register IDs (high numbers to avoid conflicts with CPU-specific ones) */
#define REG_PC          0x1000
#define REG_SP          0x1001
#define REG_FLAGS       0x1002
#define REG_A           0x1003
#define REG_B           0x1004
#define REG_X           0x1005
#define REG_Y           0x1006
#define REG_PREVIOUSPC  0x1007
#define REG_SP_CONTENTS 0x1008

/***************************************************************************
 * CPU Info Strings
 ***************************************************************************/

#define CPU_INFO_REG        0x10000000
#define CPU_INFO_FLAGS      0x40000000
#define CPU_INFO_NAME       0x40000001
#define CPU_INFO_FAMILY     0x40000002
#define CPU_INFO_VERSION    0x40000003
#define CPU_INFO_FILE       0x40000004
#define CPU_INFO_CREDITS    0x40000005
#define CPU_INFO_REG_LAYOUT 0x40000006
#define CPU_INFO_WIN_LAYOUT 0x40000007

/***************************************************************************
 * Special IRQ Lines
 ***************************************************************************/

#define IRQ_LINE_NMI    127  /* Non-maskable interrupt */

/***************************************************************************
 * Z80 Interrupt States
 ***************************************************************************/

#define Z80_INT_REQ     0x01  /* Interrupt requested */
#define Z80_INT_IEO     0x02  /* Interrupt enable out */

/***************************************************************************
 * IRQ Line States
 ***************************************************************************/

#define CLEAR_LINE      0
#define ASSERT_LINE     1
#define HOLD_LINE       2
#define PULSE_LINE      3

/***************************************************************************
 * PAIR Type - Union for byte/word access
 ***************************************************************************/

/* PAIR: Union for accessing bytes/words in machine-independent way */
typedef union {
#ifdef MSB_FIRST
    struct { UINT8 h3,h2,h,l; } b;
    struct { UINT16 h,l; } w;
#else
    struct { UINT8 l,h,h2,h3; } b;
    struct { UINT16 l,h; } w;
#endif
    UINT32 d;
} PAIR;

/***************************************************************************
 * Z80 Daisy Chain (for interrupt handling)
 ***************************************************************************/

#define Z80_MAXDAISY 4  /* Maximum daisy chain devices */

typedef struct {
    void (*reset)(int);              /* Reset callback */
    int  (*interrupt_entry)(int);    /* Entry callback */
    void (*interrupt_reti)(int);     /* RETI callback */
    int  irq_param;                  /* Callback parameter */
} Z80_DaisyChain;

/***************************************************************************
 * CPU Interface Functions (will be implemented)
 ***************************************************************************/

/* Get CPU interface for a given CPU type */
const cpu_interface *cpuintf_get_interface(int cpu_type);

/* Activator functions */
int  cpu_is_valid(int cpunum);
int  cpu_get_type(int cpunum);
void cpu_set_reset_line(int cpunum, int state);
void cpu_set_halt_line(int cpunum, int state);

#endif /* OSD_CPU_H */
