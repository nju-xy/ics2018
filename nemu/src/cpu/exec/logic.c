#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
	//TODO();
	//rtl_update_ZFSF(&id_dest->val, id_dest->width);	
	rtl_and(&t0, &id_dest->val, &id_src->val);
	//operand_write(id_dest, &t0);
	cpu.eflags.CF = cpu.eflags.OF = 0;
	rtl_update_ZFSF(&t0, id_dest->width);	
	print_asm_template2(test);
}

make_EHelper(and) {
	//TODO();
	rtl_and(&id_dest->val, &id_dest->val, &id_src->val);
	cpu.eflags.CF = cpu.eflags.OF = 0;
    operand_write(id_dest, &id_dest->val);
	//id_dest->val = at;
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
	print_asm_template2(and);
}

make_EHelper(xor) {
	//Log("%x %x %x", id_src->val, id_src2->val, id_dest->val);
	//TODO();
	rtl_xor(&id_dest->val, &id_dest->val, &id_src->val);
    operand_write(id_dest, &id_dest->val);
	cpu.eflags.CF = cpu.eflags.OF = 0;
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
    print_asm_template2(xor);
}

make_EHelper(or) {
	//TODO();
	rtl_or(&id_dest->val, &id_dest->val, &id_src->val);
    operand_write(id_dest, &id_dest->val);
	cpu.eflags.CF = cpu.eflags.OF = 0;
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
	print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
    //Log("%x >> %d", id_dest->val, id_src->val);
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
	rtl_sar(&id_dest->val, &id_dest->val, &id_src->val);
	//Log("=%x", id_dest->val);
    operand_write(id_dest, &id_dest->val);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
	rtl_shl(&id_dest->val, &id_dest->val, &id_src->val);
    operand_write(id_dest, &id_dest->val);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
	rtl_shr(&id_dest->val, &id_dest->val, &id_src->val);
    operand_write(id_dest, &id_dest->val);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t0, cc);
  operand_write(id_dest, &t0);
  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
    //TODO();
	rtl_not(&id_dest->val, &id_dest->val);
	operand_write(id_dest, &id_dest->val);
  print_asm_template1(not);
}

make_EHelper(rol) {
    //TODO();
	rtl_mv(&t0, &id_src->val);
	while(t0 != 0){
		rtl_msb(&t1, &id_dest->val, id_dest->width);
		rtl_shli(&id_dest->val, &id_dest->val, 1);
		rtl_add(&id_dest->val, &id_dest->val, &t1);
		t2 = 1;
		rtl_sub(&t0, &t0, &t2);
	}
	operand_write(id_dest, &id_dest->val);
    print_asm_template1(not);
}
