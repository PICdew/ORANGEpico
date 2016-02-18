#include "HardwareProfile.h"
#include "./USB/usb.h"

typedef	unsigned int uint;
void _BOOTROM_ delay_us(int us);

#if	0
//	memcpy�͎g���Ă��Ȃ�.

/********************************************************************
 *	code�̈�ߖ� memcpy
 ********************************************************************
 */
void _BOOTROM_ *memcpy(void *dst,const void *src,size_t size)
{
	char *t = (char*)dst;
	char *s = (char*)src;
	while(size--) {*t++=*s++;}
	return t;
}
#endif
/********************************************************************
 *	code�̈�ߖ� memset
 ********************************************************************
 */
void _BOOTROM_ *memset(void *dst,int data,size_t size)
{
	char *t = (char*)dst;
	while(size--) {*t++=data;}
	return t;
}

void wait_nloop(int n);
/********************************************************************
 *	( ms ) * 1mS �҂�.
 ********************************************************************
 */
void wait_ms(int ms)
{
	int i;
	for(i=0; i<ms; i++) {
		delay_us(1000);
	}
}

/********************************************************************
 *	( us ) * 1uS �҂�.
 ********************************************************************
 */
void _BOOTROM_ delay_us(int us)
{
	wait_nloop(us*10);
}
/********************************************************************
 *	( nloop * 1/10 ) uS �҂�. clock=40MHz�Ɖ���.
 ********************************************************************

9fc00110 <wait_nloop>:
9fc00110:	6500      	nop
9fc00112:	4cff      	addiu	a0,-1
9fc00114:	2cfd      	bnez	a0,9fc00110 <wait_nloop>
9fc00116:	e8a0      	jrc	ra

 */
//clock=40MHz�Ɖ���.	�œK���I�v�V������R���p�C�����ς������Čv��.
void _BOOTROM_ wait_nloop(int n)
{
	do {
		asm("nop");
		n--;
	}while( n != 0);
}
#if	0
//	�ȉ��̊֐��́A�T�C�Y�œK���̂��߁Acrt0.S�Ɉړ����܂���.
/********************************************************************
 *	code�̈�ߖ� INT�n�֐�.
 ********************************************************************
 */
unsigned int __attribute__((nomips16,section(".bootrom"))) INTDisableInterrupts(void)
{
	asm("di $2");	// $2 = v0
}
void __attribute__((nomips16,section(".bootrom")))  INTRestoreInterrupts(unsigned int status)
{
	if((status & 1)==0) {
		asm("di");
	}else{
		asm("ei");
	}
}
void _BOOTROM_ INTClearFlag(INT_SOURCE source)
{
	// FIXME!
}
#endif


/********************************************************************
 *	
 ********************************************************************
 */
