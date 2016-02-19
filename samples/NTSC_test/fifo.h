#ifndef	_fifo_h_
#define	_fifo_h_

#define	_MIPS32 __attribute__((nomips16,noinline))


typedef	char FIFO_t;	// FIFO�ɗ��߂�f�[�^�̌^.

/****************************************************************
 *	�� fifo �� ������o��
 ****************************************************************
 *	
 */
typedef	struct {
	FIFO_t *buf;			// FIFO�o�b�t�@.
	int    size;		// FIFO�o�b�t�@�̃T�C�Y.
	int    inPtr;		// �l�ߍ��݃|�C���^. (0 .. size-1)
	int    outPtr;		// ���o���|�C���^. (0 .. size-1)
} FIFO;

int FIFO_init(   FIFO *queue,FIFO_t *buf,int size);	//������.
int FIFO_enqueue(FIFO *queue,FIFO_t *buf,int size);	//�L���[�ɂ��߂�.
int FIFO_dequeue(FIFO *queue,FIFO_t *buf,int size);	//�L���[������o��.
int FIFO_getsize(FIFO *queue);					//�L���[�ɂ��܂��Ă���o�C�g���𓾂�.
int FIFO_getfreesize(FIFO *queue);				//�L���[�̋󂫗e�ʃo�C�g���𓾂�.

#define	FIFO_reset( queue )	(queue)->outPtr=(queue)->inPtr

/****************************************************************
 *	
 ****************************************************************
 */

#endif
