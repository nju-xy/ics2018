#include "cpu/exec.h"
extern uint32_t pio_read_l(ioaddr_t addr);
extern uint32_t pio_read_w(ioaddr_t addr);
extern uint32_t pio_read_b(ioaddr_t addr);
extern void pio_write_l(ioaddr_t addr, uint32_t data);
extern void pio_write_w(ioaddr_t addr, uint32_t data);
extern void pio_write_b(ioaddr_t addr, uint32_t data);
extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
  //TODO();
	cpu.IDTR.Limit = vaddr_read(id_src->val, 2);
	cpu.IDTR.Base = vaddr_read(id_src->val + 2, 4);
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
	//TODO();
	
	//Log("mov_r2cr %x to cr%d", id_src->val, id_dest->reg);
	if(id_dest->reg == 3)
		//operand_write(id_dest, &id_src->val);
		cpu.cr3.val = id_src->val;
	else if(id_dest->reg == 0)
		//operand_write(id_dest, &id_src->val);
		cpu.cr0.val = id_src->val;
	else 
		TODO();
	//Log("cpu.cr0 = %x, cpu.cr3 = %x", cpu.cr0.val, cpu.cr3.val);
	
	print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
	//TODO();
	//Log("mov_cr2r cr%d %x to %s", id_src->reg, cpu.cr0.val, reg_name(id_dest->reg, 4));
	if(id_src->reg == 3)
		operand_write(id_dest, &cpu.cr3.val);
	else if(id_src->reg == 0)
		operand_write(id_dest, &cpu.cr0.val);
	else 
		TODO();
	//Log("eax = %x", cpu.eax);
	print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  //TODO();
	//rtl_push(cpu.EFLAGS);
	//rtl_push(cpu.CS);
	//rtl_push(cpu.eip);
	//Log("%x %x", id_dest->val, id_src->val);
	raise_intr(id_dest->val, decoding.seq_eip);
  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
	//TODO();
	rtl_pop(&cpu.eip);
	//Log("iret, eip = %x", cpu.eip);
	rtl_j(cpu.eip);
	rtl_pop(&cpu.CS);
	//Log("iret, cs = %x", cpu.CS);
	rtl_pop(&cpu.EFLAGS);
	//Log("iret, eflags = %x", cpu.EFLAGS);
  print_asm("iret");
}

make_EHelper(in) {
	//TODO();
	switch(id_dest->width){
		case 1: id_dest->val = pio_read_b(id_src->val);  break;
		case 2: id_dest->val = pio_read_w(id_src->val);  break;
		case 4: id_dest->val = pio_read_l(id_src->val);  break;
		default: TODO();
	}
	operand_write(id_dest, &id_dest->val);
	print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
	//TODO();
	switch(id_dest->width){
		case 1: pio_write_b(id_dest->val, id_src->val);  break;
		case 2: pio_write_w(id_dest->val, id_src->val);  break;
		case 4: pio_write_l(id_dest->val, id_src->val);  break;
		default: TODO();
	}
	operand_write(id_dest, &id_dest->val);
	print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
