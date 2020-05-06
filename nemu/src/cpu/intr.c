#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
	rtl_push(&cpu.EFLAGS);
	cpu.eflags.IF = 0;
	rtl_push(&cpu.CS);
	rtl_push(&ret_addr);
	//cpu.CS = ...;
	rtl_j(vaddr_read(cpu.IDTR.Base + NO * 8, 2) + ((vaddr_read(cpu.IDTR.Base + NO * 8 +6, 2)) << 16));
}

void dev_raise_intr() {
	cpu.INTR = 1;
}
