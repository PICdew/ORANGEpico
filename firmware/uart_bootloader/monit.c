/********************************************************************
 *	�ȈՃ��j�^
 ********************************************************************
 */
#include <string.h>

#include "NVMem.h"
#include "serial1.h"

#include "hidcmd.h"
#include "monit.h"



/********************************************************************
 *	�X�C�b�`
 ********************************************************************
 */

/********************************************************************
 *	��`
 ********************************************************************
 */
#define	DEVID	0x32

#define	VER_H	1
#define	VER_L	1

#if	_ROMADRS || _APPLICATION
#define	DEV_LOADER	0
#else
#define	DEV_LOADER	1
#endif

#define	MONIT_SOF		0x01
#define	REPLY_REPORT	0xaa

//
// ===        STM32�ȊO�ł�Flash Programming ����.
//
#if	0	//ndef	SUBMDL_STM32F103

UINT	NVMemOperation(UINT nvmop);
UINT	NVMemErasePage(void* address);
UINT	NVMemWriteWord(void* address, UINT data);
UINT	NVMemWriteRow(void* address, void* data);
UINT	NVMemClearError(void);

#endif

void	reset_device(int adrs);

/********************************************************************
 *	�f�[�^
 ********************************************************************
 */

//	AVR�������ݗp�R���e�N�X�g.
uchar  page_mode;
uchar  page_addr;
uchar  page_addr_h;

//	�f�[�^����胂�[�h�̐ݒ�.
uchar   poll_mode;	// 00=���ݒ�  0xa0=�A�i���O�T���v��  0xc0=�f�W�^���T���v��
// 0xc9=run�R�}���h�^�[�~�l�[�g
uchar  *poll_addr;	//

//	�R�}���h�̕ԐM���K�v�Ȃ�1���Z�b�g����.
uchar	ToPcRdy;

uchar	puts_mode;

uchar 	puts_buf[64];
uchar 	puts_ptr;

uchar	poll_wptr;
uchar	poll_rptr;
uchar	poll_buf[256];


#define	Firm_top	0x1d000000

// 							   BMXPFMSZ:I/O(R):  FlashROM�̃T�C�Y
#define	Firm_end	(Firm_top + BMXPFMSZ)
//#define	Firm_end	0x1d008000

#define	FLASH_END_ADR	BMXPFMSZ		//���ۂɂ̓T�C�Y.
//#define	FLASH_END_ADR	0x8000

int		total_errs=0;
static  int test_adr0=0;

static  volatile char s_linecoding = 0;		//	baudrate���ύX���ꂽ.


Packet PacketFromPC;			//���̓p�P�b�g 64byte
Packet PacketToPC;				//�o�̓p�P�b�g 64byte

#define	Cmd0	PacketFromPC.cmd

/********************************************************************
 *
 ********************************************************************
 */
/*
int	align_size(int adr,int align)
{
	return 	(adr + align - 1) & (-align) ;	// align byte���E�ɍ��킹��.
}
*/
/********************************************************************
 *
 ********************************************************************
 */
static	inline	int	UARTputpacket(uchar *buf,int len)
{
	UARTwrite(buf,len);
	return len;
}

/********************************************************************
 *
 ********************************************************************
 */
int check_flash_adrs(int adrs)
{
	if( (adrs >= Firm_top)&&(adrs < Firm_end) )	return 1;	// VALID!
	return 0;	// INVALID.
}
/********************************************************************
 *	ISP���W���[�� �C���N���[�h.
 ********************************************************************
 */
//#include "usi_pic18s.h"

static void isp_command(uchar *data)
{
	uchar i;
	for (i=0; i<4; i++) {
		PacketToPC.raw[i]=usi_trans(data[i]);
	}
}

/********************************************************************
 *	FLASH�̈�(9d00_0000,9d00_0000 + ea)���̍ŏI�������݃|�C���g��T��.
 ********************************************************************
 */
int	search_flash_end(int ea)
{
	int *p  = (int *) (0x9d000000+ea);
	int cnt = ea >> 2;
	int i;
	for(i=0;i<cnt;i++) {
		p--;
		if(*p != (-1)) break;	// 0xffff_ffff ?
	}
	return (int)p;
}

/********************************************************************
 *	�ڑ��e�X�g�̕ԓ�
 ********************************************************************
 */
void cmd_echo(void)
{
	int *fl_stat = &PacketToPC.rawint[2];
	PacketToPC.raw[1]=DEVID;				// DeviceID
	PacketToPC.raw[2]=VER_L;				// version.L
	PacketToPC.raw[3]=VER_H;				// version.H
	PacketToPC.raw[4]=DEV_LOADER;			// bootloader
	PacketToPC.raw[5]=PacketFromPC.raw[1];	// ECHOBACK TEST

	fl_stat[0] = (FLASH_END_ADR);
	fl_stat[1] = (search_flash_end(FLASH_END_ADR));
	fl_stat[2] = (total_errs);

	total_errs=0;
	ToPcRdy = 1;
}

/********************************************************************
 *	�������[�ǂݏo���i32byte�ȏ�ꊇ�j
 ********************************************************************
 */
static	inline	void cmd_peek_large_block(int adrs,int size)
{
	uchar *p = (uchar *)adrs;
	UARTputpacket(p,size);
}

/********************************************************************
 *	�������[�ǂݏo��
 ********************************************************************
 */
void cmd_peek(void)
{
	Packet *t = &PacketFromPC;
	int  size = (t->size);
	int  adrs = (t->adrs);
	int    i,subcmd;
	uchar  *pb;
	ushort *ph;
	uint   *pw;

	uchar  *tb;
	ushort *th;
	uint   *tw;

	subcmd = t->subcmd;

	if(size > PACKET_SIZE) {// 60byte�ȏ�̘A���ǂݏo���͕ʏ���.
		cmd_peek_large_block(adrs,size);
		return;
	}

	switch(subcmd) {         // �ǂݏo�����w��ɉ�����������
	default:
	case MEM_BYTE:
		pb = (uchar *)adrs;
		tb = (uchar *)PacketToPC.raw;
		for(i=0; i<size; i++) {
			*tb++ = *pb++;
		}
		break;
	case MEM_HALF:
		ph = (ushort *)adrs;
		th = (ushort *)PacketToPC.rawint;
		for(i=0; i<size; i+=2) {
			*th++ = *ph++;
		}
		break;
	case MEM_WORD:
		pw = (uint *)adrs;
		tw = (uint *)PacketToPC.rawint;
		for(i=0; i<size; i+=4) {
			*tw++ = *pw++;
		}
		break;
	}

	UARTputpacket(PacketToPC.raw,PACKET_SIZE);

//	ToPcRdy = 1;
}
/********************************************************************
 *	�������[��������
 ********************************************************************
 */
void cmd_poke(void)
{
	Packet *t = &PacketFromPC;
	int  size = (t->size);
	int  adrs = (t->adrs);
	int  i,subcmd;

	uchar  *pb;
	ushort *ph;
	uint   *pw;

	uchar  *sb;
	ushort *sh;
	uint   *sw;

	subcmd = t->subcmd;

	switch(subcmd) {	// �������ݕ��w��ɉ�����������
	default:
	case MEM_BYTE:
		pb = (uchar *)adrs;
		sb = (uchar *)t->data;
		for(i=0; i<size; i++) {
			*pb++ = *sb++;
		}
		break;
	case MEM_HALF:
		ph = (ushort *)adrs;
		sh = (ushort *)t->data;
		for(i=0; i<size; i+=2) {
			*ph++ = *sh++;
		}
		break;
	case MEM_WORD:
		pw = (uint *)adrs;
		sw = (uint *)t->data;
		for(i=0; i<size; i+=4) {
			*pw++ = *sw++;
		}
		break;
	}
}

/********************************************************************
 *	FLASH�̃y�[�W�P�ʏ���.	(������Page�̐擪adrs) �����T�C�Y�͈Ö�.
 ********************************************************************
 */
void cmd_page_erase()
{
	int adrs = PacketFromPC.adrs  & 0x1fffffff;
	if( check_flash_adrs(adrs)) {
		NVMemErasePage((void*) adrs);
	}
	ToPcRdy = 1;		//�����̂��߁A�_�~�[�f�[�^��Ԃ�.
}

static	inline	void FLASH_Lock()
{
	NVMCONbits.WREN = 0;		//clear WREN bit 
}
static	inline	void FLASH_Unlock()
{
	NVMemClearError();
}
/********************************************************************
 *	FLASH�̏������݂����b�N/�A�����b�N����.(����size���[���Ȃ�A�����b�N)
 ********************************************************************
 */
static	inline	void cmd_flash_lock()
{
	if(PacketFromPC.size) {
		FLASH_Lock();
	} else {
		/* Unlock the internal flash */
		FLASH_Unlock();
	}
}

/********************************************************************
 *	FLASH�̃y�[�W�P�ʏ�������.	(�����͏�������adrs��size)
 ********************************************************************
  STM32�ł́Aword�P�ʂŏ������݂��\. (flush����s�v)

  ���̃}�C�R��(LPC�Ȃ�)�ł́A
  	���ۂɂ�RAM��̃y�[�W�o�b�t�@��word�P�ʂł��߂Ă����āA
  	�Ō��size=0���w�肵�ăy�[�W�o�b�t�@���ꔭ�������݂�������ɂȂ邱�Ƃ�����.
 

 */
void cmd_page_write()
{
	int  *p = PacketFromPC.data;
	int   i;
	int   size = (PacketFromPC.size);
	int	   adr = (PacketFromPC.adrs) & 0x1fffffff;

	// ������byte����word���ɕϊ����Ă���.
	size = (size+3) >> 2;		// size ��4byte�P�ʂɌ���.

//	if(size==0) {				// size �� 0 �̂Ƃ��͏�������flush���������.
//		FLASH_ProgramPage(adr);	// �����STM32�Ɍ���s�v(�_�~�[�֐�)
//		return ;
//	}

	for(i=0; i<size; i++) {
		if( check_flash_adrs(adr) ) {
			NVMemWriteWord((void*) adr, *p);
//			FLASH_ProgramWord(adr, *p);	// STM32�ł͒���FLUSH�ɏ���.
		}								// ���̃}�C�R���ł́A�y�[�W�o�b�t�@�ɋL�^.
		p++;
		adr+=4;
	}
}

/********************************************************************
 *	�Ԓn�w��̎��s
 ********************************************************************
 */
static	inline	void call_func(int adrs)
{
	void (*func)(void);
	func = (void (*)()) adrs;

	func();
}



/********************************************************************
 *	�Ԓn�w��̎��s
 ********************************************************************
 */
static	inline	void cmd_exec()
{
	int bootflag = (PacketFromPC.size);
	int	    adrs = (PacketFromPC.adrs);
	if(	bootflag ) {
		reset_device(adrs);
	}
	call_func(adrs);
}

#if	APPLICATION_MODE
/********************************************************************
 *	puts ��������z�X�g�ɕԑ�����.
 ********************************************************************
 */
void cmd_get_string(void)
{
	PacketToPC.raw[0] =  puts_mode;	//'p';		/* �R�}���h���s������HOST�ɒm�点��. */
	PacketToPC.raw[1] =  puts_ptr;	// ������.
	memcpy( (void*)&PacketToPC.raw[2] , (void*)puts_buf , puts_ptr & 0x3f);	//������.
	puts_ptr = 0;
	ToPcRdy = 1;
}
/********************************************************************
 *	���[�U�[��`�֐��̎��s.
 ********************************************************************
 */
void cmd_user_cmd(void)
{
	puts_ptr = 0;
	puts_mode = 'p';
	user_cmd(BSWAP32(PacketFromPC.adrs));
	puts_mode = 'q';
}
#endif

/********************************************************************
 *	1mS�P�ʂ̒x�����s��.
 ********************************************************************
 */
void cmd_wait_msec(void)
{
	uchar ms = PacketFromPC.raw[1];	// '-dN'
	if(ms) {
		wait_ms(ms);
	}
}

/********************************************************************
 *	���j�^�R�}���h��M�Ǝ��s.
 ********************************************************************
 */
static	inline void ProcessIO(void)
{
	// �ԓ��p�P�b�g����ł��邱�ƁA���A
	// �����Ώۂ̎�M�f�[�^������.
	
	//��M�f�[�^������΁A��M�f�[�^���󂯎��.
	PacketToPC.raw[0]=Cmd0;		// CMD ECHOBACK
	switch(Cmd0) {
	 case HIDASP_PEEK: 		{cmd_peek();break;}			// �������[�ǂݏo��.
	 case HIDASP_POKE: 		{cmd_poke();break;}			// �������[��������.
	 case HIDASP_JMP: 		{cmd_exec();break;}			// �ċN��.
	 case HIDASP_PAGE_ERASE:{cmd_page_erase();break;}	//Flash����.
	 case HIDASP_PAGE_WRITE:{cmd_page_write();break;}	//Flash����.
	 case HIDASP_FLASH_LOCK:{cmd_flash_lock();break;}	//FlashLock.
	 case HIDASP_TEST: 		{cmd_echo();break;}			// �ڑ��e�X�g.

#if	APPLICATION_MODE
	 case HIDASP_GET_STRING:{cmd_get_string();break;}
	 case HIDASP_USER_CMD:  {cmd_user_cmd();break;}
#endif
	 default: break;
	}
	// �K�v�Ȃ�A�ԓ��p�P�b�g���C���^���v�g�]��(EP1)�Ńz�X�gPC�ɕԋp����.
	if( ToPcRdy ) {
		UARTputpacket(PacketToPC.raw,PACKET_SIZE);
		ToPcRdy = 0;
	}
}
/********************************************************************
 *	���C������
 ********************************************************************
 */
void UARTmonit(void)
{
	int rc=0;
	while(1){
		// �擪byte��0x01�łȂ���΁A�G���[.
		int ch = Serial1GetKey();
		if(	ch != 0x01 ) return;		// Error������I��.

		do {	//	PACKET��M������҂�
			rc = UARTgetpacket(PacketFromPC.raw,PACKET_SIZE);
		}while(rc==0);

		// PACKET������.
		ProcessIO();
	}
}

/********************************************************************
 *
 ********************************************************************
 */
