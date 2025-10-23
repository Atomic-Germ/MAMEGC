/***************************************************************************
 * CPU Interface (Minimal Stub)
 ***************************************************************************/

#ifndef CPUINTRF_H
#define CPUINTRF_H

#include "osd_cpu.h"

/* Memory read/write callbacks */
typedef UINT8 (*mem_read_handler)(UINT32 address);
typedef void (*mem_write_handler)(UINT32 address, UINT8 data);

/* CPU interface functions (stubs for now) */
void cpu_setOPbase16(int cpu, unsigned val);
void cpu_set_irq_line(int cpu, int irqline, int state);
int  cpu_getactivecpu(void);

/* Memory access functions */
UINT8  cpu_readop(UINT32 address);
UINT8  cpu_readop_arg(UINT32 address);
UINT8  cpu_readmem16(UINT32 address);
void   cpu_writemem16(UINT32 address, UINT8 data);
UINT16 cpu_readport16(UINT16 port);
void   cpu_writeport16(UINT16 port, UINT16 data);

/* Change PC callback */
extern void change_pc16(unsigned pc);

#define change_pc(pc) change_pc16(pc)

#endif /* CPUINTRF_H */
