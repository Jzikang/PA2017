#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  t0 = cpu.eflags.val;
  cpu.eflags.IF = 0;
  t1 = cpu.cs;
  t2 = ret_addr;
  rtl_push(&t0);
  rtl_push(&t1);
  rtl_push(&t2);
  uint32_t address = NO * 8 + cpu.idtr.base;
  uint32_t newset = vaddr_read(address + 4, 4);
  uint32_t select = vaddr_read(address, 4);
  newset = (newset & 0xffff0000) | (select & 0xffff);
  decoding.jmp_eip = newset;
  decoding.is_jmp = 1;
}

void dev_raise_intr() {
}
