#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:{
        //printf("1\n");
	rtl_get_OF(dest);
	break;}
    case CC_B:{
        //printf("2\n");
	rtl_get_CF(dest);
	break;}
    case CC_E:{
        //printf("3\n");
	//printf("eax=0x%x\n",cpu.eax);
	rtl_get_ZF(dest);
        //printf("Neax=0x%x\n",cpu.eax);
	//printf("dest=0x%x\n",*dest);
	break;}
    case CC_BE:{
        //printf("4\n");
	rtl_get_CF(&t0);
	rtl_get_ZF(&t1);
	rtl_or(dest,&t0,&t1);
	break;}
    case CC_S:{
        //printf("5\n");
	rtl_get_SF(dest);
	break;}
    case CC_L:{
        //printf("6\n");
	rtl_get_SF(&t0);
	rtl_get_OF(&t1);
	rtl_li(dest,(t0!=t1));
	break;}
    case CC_LE:{
        //printf("7\n");
	rtl_get_ZF(&t0);
        //printf("t0=0x%x\n",t0);
	rtl_get_SF(&t1);
        //printf("t1=0x%x\n",t1);
	rtl_get_OF(&t2);
        //printf("t2=0x%x\n",t2);
        rtl_eqi(&t3,&t1,t2);
        t3=!t3;
        rtl_or(dest,&t0,&t3);
	break;}
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
