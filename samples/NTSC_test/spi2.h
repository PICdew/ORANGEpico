#ifndef _spi2_h_
#define _spi2_h_

#define	_MIPS32 __attribute__((nomips16,noinline))
#define	_ISR    __attribute__((interrupt,nomips16,noinline))

//================================================================
//	SPI Config:
#define	USE_SPI2				(1)	// SPI2 ���g�p����.
#define	SVGA_MODE				(0)	// 0:VGA 1:SVGA

#define	DDS_TEST				(0)	// DDS
#define	TEST_SPI2_TONE			(0)	// SPI2 ����������Ƀe�X�g�f�[�^��Write����.
#define	DMA_INTERRUPT			(0)	// DMA���荞�݂��g�p����.


#if	DDS_TEST
//	SPI2��TX Empty���荞�݂��g�p���ĘA����������(Audio).
#define	USE_SPI2_INTERRUPT		(1)	// SPI2 ���M���荞�݂��g�p����.
#define	USE_DMA					(0)	// DMA  ���g�p����. (0)= SPI_write�ő�p.
#else
//	DMA���g�p����1LINE���̏�������.
#define	USE_SPI2_INTERRUPT		(0)	// SPI2 ���M���荞�݂��g�p����.
#define	USE_DMA					(1)	// DMA  ���g�p����. (0)= SPI_write�ő�p.
#endif


//================================================================
#include "utype.h"

//================================================================
void	SPI2_mode(uchar mode);
void	SPI2_clock(uint speed);
void	SPI2_close();
void	SPI2_init();

uint 	SPI2_write(uint data_out);
uint 	SPI2_read(void);
//================================================================

void 	NTSC_init(void);
void 	_MIPS32 wait_ms(int ms);
void 	draw_screen(void);
void	putch_cls();

//================================================================
//	��ʃT�C�Y
#define	WIDTH32		40		//32	// 4�̔{���Ɍ���.
#define	HEIGHT		208

#define	WIDTH_DMA	 WIDTH32	//(32bit) �S�̔{���Ɍ���. WIDTH�Ɠ��l.
#define	WIDTH_WORD	(WIDTH32/sizeof(int))	// WORD��.



#endif

