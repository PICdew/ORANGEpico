/****************************************************************
 *	�� fifo �� ������o��
 ****************************************************************
int FIFO_init(   FIFO *queue,FIFO_t *buf,int size);	//������.
int FIFO_enqueue(FIFO *queue,FIFO_t *buf,int size);	//�L���[�ɂ��߂�.
int FIFO_dequeue(FIFO *queue,FIFO_t *buf,int size);	//�L���[������o��.
int FIFO_getsize(FIFO *queue);					//�L���[�ɂ��܂��Ă���v�f���𓾂�.
int FIFO_getfreesize(FIFO *queue);				//�L���[�̋󂫗e�ʗv�f���𓾂�.
 */
#include "fifo.h"

#define	_BOOTROM_  __attribute__((section(".bootrom")))

/****************************************************************
 *	���[�N
 ****************************************************************
#define	RXBUF_SIZE	1024			// ��MFIFO�T�C�Y.
static uchar rxbuf[RXBUF_SIZE];	// ��MFIFO.
 */
/****************************************************************
 *	������
 ****************************************************************
 */
int FIFO_init( FIFO *queue,FIFO_t *buf,int size)
{
	queue->buf = buf;
	queue->size = size;
	queue->inPtr = 0;
	queue->outPtr = 0;
	return 0;
}
#if	0
int FIFO_reset( FIFO *queue )
{
	queue->outPtr = queue->inPtr;
	return 0;
}
#endif
/****************************************************************
 *	�����
 ****************************************************************
 *	�����F
 *			queue   : FIFO�L���[
 *			data  : �l�ߍ��ރf�[�^.
 *			size  : �l�ߍ��ޗv�f��.
 *	�߂�l�F�l�ߍ��݂ɐ��������v�f��. (0 �������� size)
 *			�L���[�ɏ[���ȋ󂫂��Ȃ��Ƃ��͋l�ߍ��݂̓L�����Z��.
 */
int FIFO_enqueue(FIFO *queue,FIFO_t *data,int size)
{
	int i;
	int n = queue->inPtr;				// FIFO�̏������݃|�C���^.
	for(i=0; i<size; i++) {
		queue->buf[n] = *data++;
		n++;
		if(	n >= queue->size) {
			n = 0;
		}
		// FIFO���o���|�C���^�ɒǂ�������A�|�C���^���X�V���Ȃ��ŃL�����Z��.
		if(n == queue->outPtr) {
			return 0;
		}
	}
	queue->inPtr = n;
	return i;
}
/****************************************************************
 *	���܂��Ă���v�f����Ԃ�.
 ****************************************************************
 *	�߂�l�F queue �ɗ��܂��Ă���v�f��
 */
int FIFO_getsize(FIFO *queue)
{
	int n = queue->inPtr - queue->outPtr;
	if(n<0) {
		n = n + queue->size;
	}
	return n;
}
/****************************************************************
 *	�󂢂Ă���v�f����Ԃ�.
 ****************************************************************
 *	�߂�l�F queue �̋󂢂Ă���v�f��
 */
int FIFO_getfreesize(FIFO *queue)
{
	return queue->size - FIFO_getsize(queue);
}
/****************************************************************
 *	��o��
 ****************************************************************
 *	�����F
 *			queue   : FIFO�L���[
 *			result: ���o�����f�[�^�̎󂯎��ꏊ.
 *			size  : ���o�������v�f��.
 *	�߂�l�F���o���ɐ��������v�f��. (0 .. size)
 */
int FIFO_dequeue(FIFO *queue,FIFO_t *result,int size)
{
	int	n = queue->outPtr;					// FIFO�̎��o���|�C���^.
	int	i;
	for(i=0; i<size; i++) {
		if( n == queue->inPtr ) break;		// FIFO�̃g�b�v�ɒǂ�����.
		result[i] = queue->buf[n];			// ��M�������R�s�[����.
		n++;
		if(	n >= queue->size) {				// ����o�b�t�@����.
			n = 0;
		}
	}
	queue->outPtr = n;						// FIFO�̎��o���|�C���^�X�V.
	return i;
}

/******************************************************************
 *
 ******************************************************************
 */
