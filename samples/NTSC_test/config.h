/*
 *  �{�[�h�ݒ�:
 */

#ifndef _CONFIG_H_
#define	_CONFIG_H_

// ���� #define �́Aconfig.c��Fuse�ݒ�̐؂�ւ��Ɏg�p����Ă���̂Œ���.
#define	CPU_CLOCK_48MHz 1 // 48MHz����.


#ifdef	CPU_CLOCK_48MHz
// CPU�ƃy���t�F�����̓�����g��
#define SYS_FREQ (48000000)
#else
#define SYS_FREQ (40000000)
#endif


#endif
