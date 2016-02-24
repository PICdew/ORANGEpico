/********************************************************************
    NTSC�M���o�� / UART1 test
********************************************************************/

#include <plib.h>

#include "config.h"
#include "spi2.h"
#include "serial1.h"
#include "graph.h"

#ifndef	BAUDRATE
#define	BAUDRATE	500000
#endif

/** PRIVATE PROTOTYPES *********************************************/
static void InitializeSystem(void);

void NTSC_init(void);

#define	_MIPS32 __attribute__((nomips16,noinline))

_MIPS32 void ei()
{
//	crt0.S �̏������ɂāA���ł�MultiVectord�ɂ͂Ȃ��Ă���B
	INTCONSET=0x1000;		// EI PORT
//	INTEnableSystemMultiVectoredInt();	���̊֐����� INTCONSET�ݒ肪�܂܂��.
	INTEnableInterrupts();	// ei����.
}
// All Analog Pins as Digital IOs
static	void IOsetDigital()
{
	DDPCONbits.JTAGEN=0;		// check : already in system.c
	ANSELA = 0;
	ANSELB = 0;
//	ANSELC = 0;
//	TRISBCLR=0x8000;	//	pinmode(13, OUTPUT);
}

/********************************************************************
 *		Arduino��:	����������
 ********************************************************************
 */
static	inline void setup()
{
#ifdef	USE_INTERNAL_FRC_OSC	// RC ���U�̏ꍇ�͗]���ɏ��������K�v.
	InitializeFRC();
#endif

	IOsetDigital();				// �S�ăf�W�^�� �s���Ƃ��ď�����.

	mInitAllLEDs();				// LED������.
	mInitAllSwitches();			// Switch�ǂݎ�菉����.

	NTSC_init();				// NTSC �\��������.

	// UART1 ������ (NTSC_init�̂���)
	SerialConfigure(UART1, UART_ENABLE,	UART_RX_TX_ENABLED,	BAUDRATE);
	// ���荞�݋���.
	ei();
	// �O���t�B�b�N�e�X�g.
	gr_test();
}


/********************************************************************
 *		�V�X�e��: �����o��
 ********************************************************************
 */
static void Putch(int ch)
{
	// ����: UART1 �ɏo��
	Serial1WriteChar(ch);
	if(ch == '\r') {	// CR �Ȃ� LF ���ǉ�
		Serial1WriteChar('\n');
	}

	// ����: TV��� �ɏo��
	if(ch == '\r') {	// CR �Ȃ� LF �ɕύX
		ch = '\n';
	}
	gr_putch(ch);
}
/********************************************************************
 *		Arduino��:	�J��Ԃ�����
 ********************************************************************
 */
static	inline	void loop(void)
{
	// UART1: ����P��������  (�����҂�����)
	int ch = Serial1GetKey();
	// UART1: �ɃG�R�[�o�b�N����.
	Putch(ch);
}

/********************************************************************
 *		main()
 ********************************************************************
 */
int main(void)
{
	setup();		// ������:���[�U�[����
	while (1) {
		loop();		// ���[�v:���[�U�[����
	}
}


