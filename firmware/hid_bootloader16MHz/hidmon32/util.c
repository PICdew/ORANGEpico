/*********************************************************************
 *	USB�o�R�̃^�[�Q�b�g�������[�A�N�Z�X
 *********************************************************************
 *API:
int		UsbInit(int verbose,int enable_bulk, char *serial);			������.
int		UsbExit(void);												�I��.
void 	UsbBench(int cnt,int psize);					�x���`�}�[�N

void 	UsbPoke(int adr,int arena,int data,int mask);	��������
int 	UsbPeek(int adr,int arena);						1�o�C�g�ǂݏo��
int 	UsbRead(int adr,int arena,uchar *buf,int size);	�A���ǂݏo��
void	UsbDump(int adr,int arena,int size);			�������[�_���v

void 	UsbFlash(int adr,int arena,int data,int mask);	�A����������
 *
 *�����֐�:
void	dumpmem(int adr,int cnt);
void	pokemem(int adr,int data0,int data1);
void	memdump_print(void *ptr,int len,int off);
 */

#ifndef	_LINUX_
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#include "monit.h"
#include "portlist.h"
#include "util.h"
//#include "../../firmware/monitor/hidcmd.h"
#include "hidcmd.h"
#include "packet.h"
#include "hex.h"

#define	VERBOSE	0

#define	if_V	if(VERBOSE)

static	char verbose_mode = 0;
static  int  HidLength0 = PACKET_SIZE;
static  int  HidLength1 = PACKET_SIZE;
#define	ID1			1
#define	ID4			1
#define	LENGTH4		PACKET_SIZE


typedef	struct {
	uint	start,size;
} Region;

#define	AREA_MASK	0xe0000000
#define	ADDR_MASK	0x1FFFFFFF
static	Region region[]={
	{0x00000000,0x2000	},	// RAM
	{0x1D000000,0x8000	},	// FLASH
	{0x1FC00000,0x0C00	},	// BOOTROM
	{0x1F800000,0x10000	},	// PORT
	{0x1F880000,0x10000	},	// PORT
	{		  0,0		},	// END
};
static	int	   area_tab[4]={
	0x00000000,
	0x20000000,
	0x80000000,
	0xa0000000,
};

int	check_region(int addr,int size)
{
	int area = addr & AREA_MASK;
			   addr &= ADDR_MASK;
	int i;
	uint off,sz;
	for(i=0;i<4;i++) {
		if(	area_tab[i] == area) {	// �G���A�u���b�N�ɛƂ��Ă���.
			Region *r = region;		// �̈�e�[�u�����r�߂�.
			while(r->size) {		// �e�[�u�����ł͂Ȃ��Ȃ�...
				off = addr - r->start;	// �v���A�h���X�̓x�[�X�A�h���X���牽�o�C�g��(off)��?
				if(off < (r->size) ) {	// �̈�T�C�Y���Ɏ��܂��Ă���.
					//�c�ʂ�Ԃ�.
					sz = (r->size - off);	//���̗̈���̎c�ʂ��v�Z.
					if(size >= sz) {size=sz;}
					return size;
				}
				r++;
			}
		}
	}
	return -1;
}

void set_region_flash_size(int size)
{
	region[1].size = size;
}
// ReportID:4  POLLING PORT�������ςł���.
static	int	pollcmd_implemented=0;

#define	READ_BENCH			0

int disasm_print(unsigned char *buf,int size,int adr);

#define	USE_CODE_BUF		1
int code_buf_init();
int code_buf_read(int adr,int size,uchar *buf);

//	exit()
#define	EXIT_SUCCESS		0
#define	EXIT_FAILURE		1

extern	char	reboot_done;

/****************************************************************************
 *	�������[���e���_���v.
 ****************************************************************************
 *	void *ptr : �_���v�������f�[�^.
 *	int   len : �_���v�������o�C�g��.
 *	int   off : �^�[�Q�b�g���̔Ԓn�\��.
 */
void memdump_print(void *ptr,int len,int off)
{
	unsigned char *p = (unsigned char *)ptr;
	int i;	//,j,c;

	for(i=0;i<len;i++) {
		if( (i & 15) == 0 ) printf("%08lx", ((long)p - (long)ptr + off) );
		printf(" %02x",*p);p++;
		if( (i & 15) == 7 ) printf(" ");

		if( (i & 15) == 15 ) printf("\n");

		if( (i & 15) == (len-1) )
		{
#if	0	// ASCII DUMP
			printf("  ");
			for(j=0;j<len;j++) {
				c=p[j-len];
				if(c<' ') c='.';
				if(c>=0x7f) c='.';
				printf("%c",c);
			}
#endif
		}
	}
//	printf("\n");
}



/*********************************************************************
 *	AVR�f�o�C�X�ɏ����R�}���h�𑗂��āA�K�v�Ȃ猋�ʂ��󂯎��.
 *********************************************************************
 *	cmdBuf   *cmd : �����R�}���h
 *	uchar    *buf : ���ʂ��󂯎��̈�.
 *  int reply_len : ���ʂ̕K�v�T�C�Y. (0�̏ꍇ�͌��ʂ��󂯎��Ȃ�)
 *�߂�l
 *		0	: ���s
 *	   !0   : ����
 */
static int QueryAVR(cmdBuf *cmd)
{
	int rc = 0;
	rc = hidWriteBuffer(&cmd->report_id , HidLength1 );
	if(rc == 0) {
		printf("\nhidWrite error\n");
		exit(EXIT_FAILURE);
	}
	return rc;
}
/*********************************************************************
 *	�^�[�Q�b�g���̃������[���e���擾����
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int           size :�ǂݎ��T�C�Y.
 *	unsigned char *buf :�󂯎��o�b�t�@.
 *	����: HID Report Length - 1 ��蒷���T�C�Y�͗v�����Ă͂Ȃ�Ȃ�.
 */
int dumpmem(int adr,int width,int size,unsigned char *buf)
{
	uchar reply[256];
	int rc,rsize;

	memset(buf, 0xff , size);

	rsize = check_region(adr,size);
	if( rsize > 0 ) {
		//printf("getData(adr %x,rsize %x)\n",adr,rsize);
		rc = getData(adr,rsize,reply);
		if(rc!=0) return -1;
		//�ǂݎ�����������[�u���b�N�����݂̂��R�s�[�ŕԂ�.
		memcpy(buf, reply, rsize);
	}

	return size;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbBootTarget(int resetpc,int bootflag,int vector)
{
	execCode(resetpc);
	return 1;

#if	0
	cmdBuf cmd;
//	int   *cmdvect= (int*) &cmd.data;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_JMP;
	cmd.adr   = resetpc;		// ���s�J�n�Ԓn.
	cmd.size  = bootflag;		// 1�Ȃ�USB�o�X�����Z�b�g����.
//	*cmdvect  = vector;			// FM3�݂̂ŕK�v: ���zvector table�̃A�h���X.

	if(	bootflag ) {			// �ʂ̃t�@�[�����N���������ꍇ�́A�߂��Ă��Ȃ�.
		reboot_done = 1;
	}

	if( QueryAVR(&cmd) == 0) return 0;	//���s.

	return 1;
#endif
}

/*********************************************************************
 *	hostPC-->device �ւ̉���X�g���[���e�X�g.
 *********************************************************************
 */
#define TESTSIZE 60

int UsbTestTarget(int size)
{
#if	0
	cmdBuf cmd;
	static	int adr0=0;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_BOOT_RWW;	//�ꉞ�_�~�[�R�}���h�ɂȂ��Ă���.
	cmd.adr   = adr0++;
	cmd.size  = size;		// 1�Ȃ�USB�o�X�����Z�b�g����.

	printf("test size=%d\n",size);

	if( QueryAVR(&cmd) == 0) return 0;	//���s.

	if(size >= 64) {
		uchar buf[64];
		memset(buf,0,64);
		while(size>0) {
			int len = size;
			if( len >= TESTSIZE) { len = TESTSIZE; }
			hidWriteBuffer2(buf , len );
			size -= len;
		}
	}

	UsbPeek(0,MEM_BYTE);
#endif
	return 1;
}

/*********************************************************************
 *	device-->hostPC �ւ̏��X�g���[���e�X�g.
 *********************************************************************
 */
int UsbTestUpload(int size)
{
	uchar buf[4096];	// 1080.
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_SAMPLING;
	cmd.subcmd= 1;	// analog.
	cmd.size  = size;
	cmd.adr   = 7;	// sample rate (0=1X 7=256X)

//	printf("test size=%d\n",size);

	if( QueryAVR(&cmd) == 0) return 0;	//���s.

	hidReadBuffer(buf,size);
	memdump_print(buf,16,0);		//���ʈ�.
	return 1;
}

int	hidGetChars();
/*********************************************************************
 *
 *********************************************************************
 */
int UsbExecUser(int arg)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_USER_CMD;
	cmd.adr   = arg;

	if( QueryAVR(&cmd) == 0) return 0;	//���s.

	hidGetChars();
	printf("\n* Terminate.\n");

	return 1;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbReadString(uchar *buf)
{
	cmdBuf cmd;
	int rc;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_GET_STRING;

	buf[0]=0;
	if( QueryAVR(&cmd) == 0) return 0;	//���s.


	rc = hidReadBuffer(buf+1 , HidLength0);
	if(rc == 0) {
		printf("\nhidRead error \n");
		exit(EXIT_FAILURE);
	}

	return 1;
}

/*********************************************************************
 *
 *********************************************************************
 */
int UsbPutKeyInput(int key)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
		cmd.cmd   = HIDASP_KEYINPUT;
		cmd.size  = key;

	if( QueryAVR(&cmd) == 0) return 0;	//���s.
	return 1;
}

/*********************************************************************
 *	�^�[�Q�b�g���̃������[���e(1�o�C�g)������������.
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int          data0 :�������݃f�[�^.      (OR)
 *	int          data1 :�������݃r�b�g�}�X�N.(AND)
 *����
 *	pic32mx��bootloader�̓������[�ߖ�̂��� DWORD�A�N�Z�X�̂ݎ�������Ă���.
 *	����āAmask�͎�������Ȃ�.
 *	char,short�̃A�N�Z�X���s���ꍇ��read modified�ɂ���K�v������.
 */
int	pokemem(int adr,int arena,int data0,int data1)
{
	int buf[64/4];
	char *byte=(char *)buf;
	int off = adr & 3;	//�[��.
	int size = 4;
	if(arena == MEM_BYTE) {
		int rc = UsbRead(adr & (-4),MEM_WORD,byte,size);
		byte[off]=data0;
	}else if(arena == MEM_HALF) {
		int rc = UsbRead(adr & (-4),MEM_WORD,byte,size);
		byte[off]=data0;		// FIXME! SHORT WORD!
	}else{
		buf[0]=data0;
	}
	return putData(adr & (-4),size,(char*) buf);

#if	0
	int 					size = 1;
	if(arena==MEM_HALF) 	size = 2;
	if(arena==MEM_WORD) 	size = 4;

	buf[0]=data0;

	printf("putData(%x %d)\n",adr,size);

	return putData(adr,size,(char*) buf);
#endif
}

void hid_read_mode(int mode,int adr)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_SET_MODE;
	cmd.size  = mode;
	cmd.adr   = adr;
	QueryAVR(&cmd);

}

int hid_read(int cnt)
{
	uchar buf[256];	// dummy
	int rc;
	rc = hidReadBuffer(buf , HidLength0 );
	if(rc == 0) {
		printf("\nhidRead error \n");
		exit(EXIT_FAILURE);
	}
	return rc;
}

/*********************************************************************
 *	�^�[�Q�b�g�Ƃ̐ڑ��`�F�b�N(�x���`�}�[�N�����˂�)
 *********************************************************************
 *	int i : 0�`255 �̐��l.
 *�߂�l
 *		�G�R�[�o�b�N���ꂽ i �̒l.
 */
int hid_ping(int i)
{
 	cmdBuf cmd;
	uchar buf[256];	// dummy
	int rc;

	memset(&cmd,i,sizeof(cmdBuf));
	cmd.cmd   = CMD_PING ;
	QueryAVR(&cmd);

	rc = hidReadBuffer(buf+1 , HidLength0 );
	if(rc == 0) {
		printf("\nhidRead error\n");
		exit(EXIT_FAILURE);
	}
	// +00        +01      +02     +03     +04     +05        +06
	// [ReportID] [CMD]  [DEV_ID] [VER_L] [VER_H] [BOOTFLAG] [Echoback]
	//		DEV_ID = 0xa3 (ARMCortexM3)
	//		VER_L  = 0x01
	//		VER_H  = 0x01
	//		BOOTFLAG  0x01=bootloader 0x00=application

	return buf[6] & 0xff;
}
/*********************************************************************
 *	�]�����x�x���`�}�[�N
 *********************************************************************
 *	int cnt  : PING��ł�.
 *	int psize: PING�p�P�b�g�̑傫��(���݂̓T�|�[�g����Ȃ�)
 */
void UsbBench(int cnt,int psize)
{
	int i,n,rc;
	int time1, rate;
#ifdef __GNUC__
	long long total=0;
#else
	long total=0;
#endif
	int nBytes;
	int time0;

#if		READ_BENCH
	nBytes  = HidLength1-1 ;				//�o��p�P�b�g�̂݌v��.
#else
	nBytes  = HidLength1-1 + HidLength1-1 ;	//���݌Œ�.
#endif
	// 1���Ping�� 63byte��Query��63�o�C�g�̃��^�[��.

   	printf("hid write start\n");
	time0 = clock();
#if		READ_BENCH
	hid_read_mode(1,0);
#endif

	for(i=0;i<cnt;i++) {
		n = i & 0xff;
#if		READ_BENCH
		rc = hid_read(i);
#else
		rc = hid_ping(n);
		if(rc != n) {
			printf("hid ping mismatch error (%x != %x)\n",rc,n);
			exit(EXIT_FAILURE);
		}
#endif
		total += nBytes;
	}

#if		READ_BENCH
	hid_read_mode(0,0);
#endif

	time1 = clock() - time0;
	if (time1 == 0) {
		time1 = 2;
	}
	rate = (int)((total * 1000) / time1);

	if (total > 1024*1024) {
	   	printf("hid write end, %8.3lf MB/%8.2lf s,  %6.3lf kB/s\n",
	   		(double)total/(1024*1024), (double)time1/1000, (double)rate/1024);
	} else 	if (total > 1024) {
	   	printf("hid write end, %8.3lf kB/%8.2lf s,  %6.3lf kB/s\n",
	   		 (double)total/1024, (double)time1/1000, (double)rate/1024);
	} else {
	   	printf("hid write end, %8d B/%8d ms,  %6.3lf kB/s\n",
	   		(int)total, time1, (double)rate/1024);
   	}
}
/*********************************************************************
 *	�^�[�Q�b�g���������[���_���v����.
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int            cnt :�ǂݎ��T�C�Y.
 */
void UsbDump(int adr,int arena,int cnt)
{
	unsigned char buf[16];
	int size;
	int rc,rsize;
	while(cnt) {
		size = cnt;
		if(size>=16) size = 16;

		rsize = check_region(adr,size);
		if(	rsize <= 0) break;

		rc = dumpmem(adr,arena,rsize,buf);	//�������[���e�̓ǂݏo��.
		memdump_print(buf,rsize,adr);		//���ʈ�.

		adr += size;
		cnt -= size;
	}
}

/*********************************************************************
 *	�^�[�Q�b�g���������[���t�A�Z���u������.
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int            cnt :�ǂݎ��T�C�Y.
 */
int UsbDisasm (int adr, int arena, int cnt)
{
	unsigned char buf[16];
	int size = 8;
	int done = 0;
	int endadr = adr + cnt;
	int rc,disbytes;

	code_buf_init();

	while(adr < endadr) {
		int rsize = check_region(adr,4);
		if(	rsize < 0) break;

#if	USE_CODE_BUF		
		rc = code_buf_read(adr,size,buf);	//�������[���e�̓ǂݏo��.
#else
		rc = dumpmem(adr,AREA_PGMEM,size,buf);	//�������[���e�̓ǂݏo��.
#endif
		if(rc != size) return done;
		disbytes = disasm_print(buf,size,adr);	//���ʈ�.
		adr += disbytes;
		done+= disbytes;
	}
	return done;
}

/*********************************************************************
 *	�^�[�Q�b�g���������[�A���ǂݏo��
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	uchar         *buf :�ǂݎ��o�b�t�@.
 *	int           size :�ǂݎ��T�C�Y.
 *�߂�l
 *	-1    : ���s
 *	���̒l: ����.
 */
int UsbRead(int adr,int arena,uchar *buf,int size)
{
	int rc = size;
	int len;
	while(size) {
		int rc1;
		len = size;
		if(len >= 32) len = 32;
		rc1 = dumpmem(adr,arena,len,buf);
		if( rc1!= len) {
			return -1;
		}
		adr  += len;	// �^�[�Q�b�g�A�h���X��i�߂�.
		buf  += len;	// �ǂݍ��ݐ�o�b�t�@��i�߂�.
		size -= len; 	// �c��T�C�Y�����炷.
	}
	return rc;
}
/*********************************************************************
 *	�^�[�Q�b�g���������[1�o�C�g�ǂݏo��
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *�߂�l
 *	�������[���e�̒l.
 *����
 *	pic32mx��bootloader�̓������[�ߖ�̂��� DWORD�A�N�Z�X�̂ݎ�������Ă���.
 *	�Ȃ̂ŁAchar,short�̃A�N�Z�X���s���ꍇ��DWORD���E����4byte�ǂ�ŁA�K�v�ӏ�
 *	�����o���K�v������.
 */
int UsbPeek(int adr,int width)
{
	unsigned char buf[16];
	int off= (adr & 3);		//�[��.
	int size=4;
	int rc = UsbRead(adr & (-4),width,buf,size);
	if( rc != size) {
		return -1;
	}
	switch(width) {
	  default:
	  case MEM_BYTE:
		return buf[off+0];
	  case MEM_HALF:
		return buf[off+0] | (buf[off+1]<<8);
	  case MEM_WORD:
		return buf[0] | (buf[1]<<8)| (buf[2]<<16)| (buf[3]<<24);
	}
}

static	int	poll_port = 0;

/*********************************************************************
 *	�^�[�Q�b�g���������[1�o�C�g�A���ǂݏo���̃Z�b�g�A�b�v
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *�߂�l
 *	�������[���e�̒l.
 */
int UsbSetPoll_slow(int adr,int arena)
{
	poll_port = adr;
	return 1;
}
/*********************************************************************
 *	�^�[�Q�b�g���������[1�o�C�g�A���ǂݏo��
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *�߂�l
 *	�������[���e�̒l.
 */
int UsbPoll_slow()
{
	return UsbPeek(poll_port,MEM_WORD);
}

static	int	chkSyncCmd(uchar *buf,int i)
{
	if(buf[1]==CMD_PING) {
		if((buf[2]==0xa3)) {		// ARM-M3
		// +00        +01      +02     +03     +04     +05       +06
		// [ReportID] [CMD]  [DEV_ID] [VER_L] [VER_H] [DEVCAPS] [Echoback]
			if(buf[6]==i) return 1;
		}
	}
	return 0;	// error
}

int	UsbSyncCmd(int cnt)
{
 	cmdBuf cmd;
	uchar buf[128];	// dummy
	int rc;
	int i,c,fail,retry;

	for(i=0x80;i<0x80+cnt;i++) {
		c = i & 0xff;
		memset(&cmd,c,sizeof(cmdBuf));
		cmd.cmd   = CMD_PING ;
		QueryAVR(&cmd);

		fail=0;
		for(retry=0;retry<16;retry++) {
			rc = hidReadBuffer( buf+1 , HidLength0 );
			if(rc == 0) {
				printf("\nhidRead error\n");
				exit(EXIT_FAILURE);
				return -1;
			}
			if( chkSyncCmd(buf,c) ) {
				fail=0;				// OK.
				break;
			}else{
				fail=1;				// Retry!
			}
		}
		if(fail) {
			printf("\nhidRead Sync Error\n");
			exit(EXIT_FAILURE);
			return -1;
		}
	}
if_V	printf("hidRead Sync OK.\n");
	return 0;
}
/*********************************************************************
 *	�^�[�Q�b�g���������[1�o�C�g�A���ǂݏo���̃Z�b�g�A�b�v
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *�߂�l
 *	�������[���e�̒l.
 */
int UsbSetPoll(int adr,int mode)
{
//	char buf[128];
//	char rd_buf[128];

	if( pollcmd_implemented == 0 ) {
		return UsbSetPoll_slow(adr,0);
	}

	hid_read_mode(mode,adr);

//	SYNC����.
	if(mode==0) {
		UsbSyncCmd(1);
	}
	return 1;	// OK.
}
/*********************************************************************
 *	�^�[�Q�b�g���������[1�o�C�g�A���ǂݏo��
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *�߂�l
 *	�������[���e�̒l.
 */
int UsbPoll(uint *buf)
{
	int rc = UsbPoll_slow();
	return rc;
}
int UsbPollAll(uint *buf)
{
	cmdBuf cmd;
	int rc;

	memset(&cmd,0,sizeof(cmdBuf));
	cmd.cmd   = HIDASP_PORT_SENSE;

	if( QueryAVR(&cmd) == 0) return 0;	//���s.

	rc = hidReadBuffer(buf , HidLength0);
	if(rc == 0) {
		printf("\nhidRead error \n");
		exit(EXIT_FAILURE);
	}
	return rc;
}


int UsbAnalogPoll(int mode,uchar *abuf)
{
	int rc;
	hidCommand(HIDASP_SET_PAGE,mode,0,0);	// Set Page mode
	if(mode == 0xa2) {
		Sleep(3);
	}
	rc = hidReadPoll(abuf , LENGTH4 ,ID4);
//	Sleep(100);
	return rc;
}


/*********************************************************************
 *
 *********************************************************************
 */
int 	UsbEraseTargetROM(int adr,int size)
{
	cmdBuf cmd;
//	printf("adrs = %x size=%x\n",adr,size);

	cmd.cmd   = HIDASP_PAGE_ERASE;
	cmd.size  = size;
	cmd.adr   = adr;

	if( QueryAVR(&cmd) == 0) return 0;	//���s.
	return size;
}

/*********************************************************************
 *	�^�[�Q�b�g����Flash���e(32�o�C�g�܂�)������������.
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int           data :�������݃f�[�^.
 *	int           mask :�������ݗL���r�b�g�}�X�N.
 *����
 *  mask  = 0 �̏ꍇ�͑S�r�b�g�L�� (����������)
 *	mask != 0 �̏ꍇ�́Amask�r�b�g1�ɑΉ�����data�̂ݍX�V���A���͕ύX���Ȃ�.
 */
int UsbFlash(int adr,int arena,uchar *buf,int size)
{
	cmdBuf cmd;
	int size0=size;
	
	if(size <= 48) {
		memcpy(cmd.data,buf,size);
	}
	cmd.cmd   = HIDASP_PAGE_WRITE;
	cmd.size  = size;
	cmd.adr   = adr;

	if( QueryAVR(&cmd) == 0) return 0;	//���s.

	if(size > 48) {
		while(size>0) {
			int len = size;
			if( len >= 60) { len = 60; }
			hidWriteBuffer2(buf , len );
			buf  += len;
			size -= len;
		}
	}
	return size0;
}

/*********************************************************************
 *	�^�[�Q�b�g����Flash��Lock
 *********************************************************************
 */
int UsbFlashLock(int lockf)
{
	cmdBuf cmd;

	memset(&cmd,0,sizeof(cmdBuf));
		cmd.cmd   = HIDASP_FLASH_LOCK;
		cmd.size  = lockf;

	if( QueryAVR(&cmd) == 0) return 0;	//���s.
	return 1;
}
/*********************************************************************
 *	�^�[�Q�b�g���̃������[���e(1�o�C�g)������������.
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int           data :�������݃f�[�^.
 *	int           mask :�������ݗL���r�b�g�}�X�N.
 *����
 *  mask  = 0 �̏ꍇ�͑S�r�b�g�L�� (����������)
 *	mask != 0 �̏ꍇ�́Amask�r�b�g1�ɑΉ�����data�̂ݍX�V���A���͕ύX���Ȃ�.
 *���ӂQ
 *	pic32mx��bootloader�̓������[�ߖ�̂��� DWORD�A�N�Z�X�̂ݎ�������Ă���.
 *	����āAmask�͎�������Ȃ�.
 *	char,short�̃A�N�Z�X���s���ꍇ��read modified�ɂ���K�v������.
 *	
 */
int	usbPoke(int adr,int arena,int data,int mask)
{
	return pokemem(adr,arena,data,0);

#if	0
	int data0 ,data1;
	if(mask == 0) {
		// �}�X�N�s�v�̒�����.
		data0 = data;
		data1 = 0;
	}else{
		// �}�X�N�������܂ޏ�������.
		// �������݃f�[�^�̗L�������� mask �̃r�b�g�� 1 �ɂȂ��Ă��镔���̂�.
		// mask �̃r�b�g�� 0 �ł��镔���́A���̏���ێ�����.

		data0 = data & mask;	// OR���.
		data1 = 0xff &(~mask);	// AND���.

		// �}�X�N�������݂̃��W�b�N�͈ȉ�.
		// *mem = (*mem & data1) | data0;
	}

	return pokemem(adr,arena,data0,data1);
#endif
}
/*********************************************************************
 *	�^�[�Q�b�g���̃������[���e(1�o�C�g)������������.
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int           data :�������݃f�[�^.
 *	int           mask :�������ݗL���r�b�g�}�X�N.
 *����
 *  mask  = 0 �̏ꍇ�͑S�r�b�g�L�� (����������)
 *	mask != 0 �̏ꍇ�́Amask�r�b�g1�ɑΉ�����data�̂ݍX�V���A���͕ύX���Ȃ�.
 */
int UsbPoke(int adr,int arena,int data,int mask)
{
	return usbPoke(adr,arena,data,mask);
}
/*********************************************************************
 *	1�r�b�g�̏�������
 *********************************************************************
 *	int            adr :�^�[�Q�b�g���̃������[�A�h���X
 *	int          arena :�^�[�Q�b�g���̃������[���(���ݖ��g�p)
 *	int            bit :�������݃f�[�^(1bit) 0 �������� 1
 *	int           mask :�������ݗL���r�b�g�}�X�N.
 */
void UsbPoke_b(int adr,int arena,int bit,int mask)
{
	int data=0;
	if(mask == 0) {
		//1�o�C�g�̏�������.
		UsbPoke(adr,arena,bit,0);
	}else{
		//1�r�b�g�̏�������.
		if(bit) data = 0xff;
		UsbPoke(adr,arena,data,mask);
	}
}

/*********************************************************************
 *
 *********************************************************************
 */
void local_init(void)
{
}
/*********************************************************************
 *
 *********************************************************************
 */
void UsbCheckPollCmd(void)
{
#if	0
	if((UsbGetDevCaps(NULL,NULL)) == 1) {	// bootloader�ł���.
		pollcmd_implemented = 0;			// ��������Ă��Ȃ�.
	}else{
		pollcmd_implemented = 1;			// ��������Ă���.
	}
#endif
}

#if	0
/*********************************************************************
 *	������
 *********************************************************************
 */
int UsbInit(int verbose,int enable_bulk, char *serial,char *baud)
{
	int type = 0;
	verbose_mode = verbose;
	if(	hidasp_init(type,serial,baud) != 0) {
		if (verbose) {
	    	fprintf(stderr, "error: [%s] device not found!\n", serial);
    	}
//    	Sleep(1000);
    	return 0;
	} else {
		local_init();
		UsbCheckPollCmd();
		return 1;	// OK.
	}
}
/*********************************************************************
 *	�I��
 *********************************************************************
 */
int UsbExit(void)
{
	hidasp_close();
	return 0;
}
#endif