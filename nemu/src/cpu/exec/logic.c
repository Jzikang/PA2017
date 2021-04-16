#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0,&id_dest->val, &id_src->val);
  //printf("0x%x 0x%x\n",id_dest->val,id_src->val);
  //printf("%#010x\n",t0);
  //operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_li(&t1,0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
  //printf("0x%x 0x%x\n",id_dest->val,id_src->val);
  //printf("%#010x\n",t0);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_li(&t1,0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t0);
  rtl_li(&t1,0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  
  rtl_update_ZFSF(&t0, id_dest->width);
  //printf("xeax=0x%x\n",cpu.eax);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest,&t0);
  rtl_li(&t1,0);
  rtl_set_CF(&t1); 
  rtl_set_OF(&t1);
  rtl_update_ZFSF(&t0, id_dest->width);
  //printf("eax=0x%x\n",cpu.eax);
  print_asm_template2(or);
}

make_EHelper(rol) {
  rtl_shl(&t1,&id_dest->val,&id_src->val);
  rtl_shri(&t2,&id_dest->val,id_dest->width*8-id_src->val);
  rtl_or(&t0,&t1,&t2);
  operand_write(id_dest,&t0);
  
  print_asm_template2(rol);
}

make_EHelper(sar) {
  rtl_sar(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  //printf("Eax=0x%x\n",cpu.eax);
  rtl_setcc(&t2, subcode);
  //printf("t2=0x%x\n",t2);
  operand_write(id_dest, &t2);
  //printf("Neax=0x%x\n",cpu.eax);
  //printf("al=0x%x\n",cpu.gpr[0]._8[0]);
  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtl_mv(&t2,&id_dest->val);
  rtl_not(&t2);
  operand_write(id_dest, &t2);

  print_asm_template1(not);
}
