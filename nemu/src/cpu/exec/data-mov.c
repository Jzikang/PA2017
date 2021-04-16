#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);;

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_src->val);
  operand_write(id_dest,&id_src->val);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  t1 = cpu.esp;
  t0 = cpu.eax;
  rtl_push(&t0);
  t0 = cpu.ecx;
  rtl_push(&t0);
  t0 = cpu.edx;
  rtl_push(&t0);
  t0 = cpu.ebx;
  rtl_push(&t0);
  rtl_push(&t1);
  t0 = cpu.ebp;
  rtl_push(&t0);
  t0 = cpu.esi;
  rtl_push(&t0);
  t0 = cpu.edi;
  rtl_push(&t0);

  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&t0);
  cpu.edi = t0;
  rtl_pop(&t0);
  cpu.esi = t0;
  rtl_pop(&t0);
  cpu.ebp = t0;
  rtl_pop(&t0); // throw esp
  rtl_pop(&t0);
  cpu.ebx = t0;
  rtl_pop(&t0);
  cpu.edx = t0;
  rtl_pop(&t0);
  cpu.ecx = t0;
  rtl_pop(&t0);
cpu.eax = t0;

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
	if(cpu.gpr[0]._16<0){
		cpu.gpr[1]._16=0xffff;}
	else{
		cpu.gpr[1]._16=0x0;}
  }
  else {
	if(cpu.eax<0){
                rtl_li(&cpu.edx,0xffffffff);}
        else{
                rtl_li(&cpu.edx,0x0);}

  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
	rtl_lr_b(&t0,R_AL);
	rtl_sext(&t0,&t0,1);
	rtl_sr_w(R_AX,&t0);
  }
  else {
	rtl_lr_w(&t0,R_AX);
        rtl_sext(&t0,&t0,2);
        rtl_sr_l(R_EAX,&t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
