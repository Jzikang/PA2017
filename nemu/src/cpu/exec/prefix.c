#include "cpu/exec.h"

make_EHelper(real);

make_EHelper(operand_size) {
  decoding.is_operand_size_16 = true;
  //printf("O1=0x%x\n",eip);
  //printf("O2=0x%x\n",decoding.seq_eip);
  exec_real(eip);
  //printf("N1=0x%x\n",eip);
  //printf("N2=0x%x\n",decoding.seq_eip);
  decoding.is_operand_size_16 = false;
}
