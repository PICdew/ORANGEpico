/********************************************************************
 *	GRAPHIC SUBROUTINES
 ********************************************************************
 */
#include <stdlib.h>
#include <string.h>
#include <plib.h>

#include "utype.h"
#include "spi2.h"
#include "font8.h"

#define	M_PI	3.14159
#define SWAP(x,y)  {int t; t=(x); (x)=(y); (y)=t; }

/********************************************************************
 *	�錾
 ********************************************************************
 */

//	dot size
#define	WIDTH		(WIDTH32 * 8)
//#define	HEIGHT		216		spi2.h�Œ�`

//	word size
#define	STRIDE		(WIDTH / 32)	// DMA 1���C������32bit WORD��.

//	bit address
#define	BITMASK(x)	( ((uint)0x80000000) >> (x & 31) )

#define	CWIDTH		(WIDTH/8)	//32
#define	CHEIGHT		(HEIGHT/8)	//25
#define	CSTRIDE		(CWIDTH)	//������


extern uint	txferTxBuff[];
// �X�N���[���J�[�\�����W
static int sx=0;
static int sy=0;

/********************************************************************
 *	
 ********************************************************************
 */
//	====================================================
//	(x,y)���W����s�N�Z���o�b�t�@�̃|�C���^*p�𓾂�.
//	====================================================
int *p_adr(int x,int y)
{
	if( ((uint)x < WIDTH) && ((uint)y < HEIGHT) ) {
		return &txferTxBuff[ y * STRIDE + (x>>5) ];	// (x/32)
	}
	return 0;
}
//	====================================================
//	(x,y)���W�Ƀh�b�g��ł�.
//	====================================================
void pset(int x,int y)
{
	int *p=p_adr(x,y);
	if(p) *p |= BITMASK(x);
}
//	====================================================
//	(x,y)���W�̃h�b�g������.
//	====================================================
void preset(int x,int y)
{
	int *p=p_adr(x,y);
	if(p) *p &= ~BITMASK(x);
}
//	====================================================
//	(x,y)���W�̃h�b�g�𔽓].
//	====================================================
void prev(int x,int y)
{
	int *p=p_adr(x,y);
	if(p) *p ^= BITMASK(x);
}

#define	gr_pset(x,y,c)	pset(x,y)
//	====================================================
//	������`�悷��.
//	====================================================
void gr_line(int x1,int y1,int x2,int y2,int c)
{
	int px,py;		/* �łׂ��_ */
	int r;			/* �덷���W�X�^ */
	int dx,dy,dir,count;

	if(x1 > x2) {SWAP(x1,x2);SWAP(y1,y2);}

	px=x1;py=y1;	/* �J�n�_ */
	dx=x2-x1;	/* �f���^ */
	dy=y2-y1;dir=1;
	if(dy<0) {dy=-dy;dir=-1;} /* ���̌X�� */

	if(dx<dy) {	/* �f���^���̕����傫���ꍇ */
		count=dy+1;
		r=dy/2;
		do {
			gr_pset(px,py,c);py+=dir;
			r+=dx;if(r>=dy) {r-=dy;px++;}
		} while(--count);
	} else {	/* �f���^���̕����傫���ꍇ */
		count=dx+1;
		r=dx/2;
		do {
			gr_pset(px,py,c);px++;
			r+=dy;if(r>=dx) {r-=dx;py+=dir;}
		} while(--count);
	}
}

//	====================================================
//	�~(cx,cy),���ar,�Fc ��`��
//	====================================================
void gr_circle( int cx,int cy,int r,int c)
{
	int  x,y;
	int  xr,yr;

	if(r==0) return;
	x=r * r;y=0;
	do {
		xr= x / r;
		yr= y / r;
		gr_pset(cx+xr,cy+yr,c);
		gr_pset(cx-xr,cy+yr,c);
		gr_pset(cx-xr,cy-yr,c);
		gr_pset(cx+xr,cy-yr,c);

		gr_pset(cx+yr,cy+xr,c);
		gr_pset(cx-yr,cy+xr,c);
		gr_pset(cx-yr,cy-xr,c);
		gr_pset(cx+yr,cy-xr,c);

		x += yr;
		y -= xr;
	}while( x>= (-y) );
}
#if	0
//	���������_���C�u�����������N�����̂ŁA����
//	====================================================
//	�~��(cx,cy),���ar,�Fc,�p�x(begin,end) ��`��
//	====================================================
void gr_circle_arc( int cx,int cy,int rx,int ry,int c,int begin,int end)
{
	float x,y,rad,t,td;
	int xr,yr;
	if(rx>ry) td = rx;
	else	  td = ry;

	td = (360/6.28) / (td * 1.2);

	for(t=begin;t<end;t=t+td) {
		rad = (t * M_PI *2 ) / 360.0;
		x =  cos(rad);
		y =  sin(rad);
		xr = x * (float)rx;
		yr = y * (float)ry;
		gr_pset(cx-xr,cy-yr,c);
	}
}
#endif


//	====================================================
//	(cx,cy)�������W����s�N�Z���o�b�t�@��byte�|�C���^*p�𓾂�.
//	====================================================
uchar *ch_adr(int cx,int cy)
{
	uchar *t = (uchar*)txferTxBuff;
	if( ((uint)cx < CWIDTH) && ((uint)cy < CHEIGHT) ) {
		t += (cy*(8*CWIDTH)+(cx & 0xfffc)); // 4�����P�ʂ� 1234 --> 4321 �ɂȂ� (DMA Endian���)
		t += (3 -           (cx & 0x0003));
		return t;
	}
	return 0;
}

//	====================================================
//	ASCII����(ch) ��8x8�t�H���g�f�[�^(8byte)�𓾂�.
//	====================================================
uchar *get_font_adr(int ch)
{
	return &font8[ (ch & 0xff) * 8 ];
}

void gr_scrollup(int y)
{
	y *= 8;

	int *t = txferTxBuff;
	int *s = t + (y * STRIDE);
	int i;
	int m = (HEIGHT - y) * STRIDE;

	for(i=0;i<m;i++) *t++ = *s++;
}

void gr_crlf(void)
{
	sx=0;
	sy++;
	if(sy>=CHEIGHT) {
		sy=CHEIGHT-1;
		gr_scrollup(1);
	}
}

void gr_curadv(void)
{
	sx++;
	if(sx>=CWIDTH) {
		gr_crlf();
	}
}

void gr_locate(int cx,int cy)
{
	sx=cx;
	sy=cy;
}

//	====================================================
//	�������W(cx,cy)��ASCII����(ch)���O���t�B�b�N�`��
//	====================================================
void gr_putch(int cx,int cy,int ch)
{
	uchar *t = ch_adr(cx,cy);
	uchar *font = get_font_adr(ch);

	if(t) {
		int i;
		for(i=0;i<8;i++) {
			*t = *font++;
			t += CSTRIDE;
		}
	}
}

//	====================================================
//	�������W(cx,cy)��ASCII����(ch)���O���t�B�b�N�`��
//	====================================================
void gr_puts(char *str)
{
	while(1) {
		int ch = *str++;
		if( ch==0 ) break;
		if( ch=='\n' ){
			gr_crlf();
		}else{
			gr_putch(sx,sy,ch);
			gr_curadv();
		}
	}
}

//	====================================================
//	�O���t�B�b�N�e�X�g.
//	====================================================
void gr_test()
{
	int x,y;
	for(x=0;x<HEIGHT;x+=16) {
		gr_line(x,0,WIDTH,x,1);
	}
	gr_circle(WIDTH/2,HEIGHT/2,HEIGHT/2,1);
//	gr_circle_arc(WIDTH/2,HEIGHT/2,HEIGHT/2,HEIGHT/2,60,140,1);

	gr_locate(0,0);
	int i;
	for(i=0;i<(CHEIGHT-1);i++)
		gr_puts("  Hello,World.\n");
}

