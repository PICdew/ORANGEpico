---------------------------------------------------------------------
	PIC32MX �p LED blinker �T���v��
---------------------------------------------------------------------

�� �T�v

- PIC32MX �� LED �_�ł��s���܂��B


�� �z��         PIC32MX220F032B 8MHz�����g�p.


                3.3V
                 |
                 *------10��--------------+
                10k                       |
                 |       ___    ___       | 0.1u
   ��   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
   �C   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
   �^�[ -->  --PGC3/RA1 [3       26] RB15--1k��-------|��|--GND
                    RB0 [4       25] RB14
                    RB1 [5       24] RB13
               SDA2/RB2 [6       23] 
               SCL2/RB3 [7       22] 
    Xtal     GND----Vss [8       21]          +10uF
 +-----------------OSC1 [9       20] Vcap------||---GND
 *--|��|--*--------OSC2 [10      19] Vss------------GND
 |  8MHz  |    U1TX/RB4 [11      18] RB9                        *--10k------3.3V
 22pF    22pF  U1RX/RA4 [12      17] RB8                        |
 |        |   3.3v--Vdd [13      16] RB7 -----------------------*--BOOT_SW--GND
 |        |         RB5 [14      15] 
 GND    GND              ~~~~~~~~~~


-  �Ή�PIC�� MX220 �ȊO�ɁAMX250 , MX170 �Ȃǂ�OK�ł��B(����HEX���g�p�ł��܂�)
-  RB7��BOOT_SW , MCLR��RESET_SW���������Ă��������B
-  PIC32MX220F032B �� RB15 �ɐڑ����ꂽLED ��_�ł��܂��B


            
�� �J���� �\�z
            
- �i�P�jWindows XP / Vista / 7 / 8.1 / 10 �̂ǂꂩ��p�ӂ��܂��B(64bit OK)

- �i�Q�jPinguino-11 ���_�E�����[�h���ăC���X�g�[�����܂��B
        http://www.pinguino.cc/download.php


�� �R���p�C�����@

 -    setenv.bat �����s���āAPinguino��gcc�Ɏ��s�p�X��ʂ��܂��B
 -    ���ӁF���̊�(MinGW,Cygwin��)�� make.exe �ł̓r���h�ł��܂���B

 -    make �����s���Ă��������B

�� �t�@�[���E�F�AHEX�̏������ݕ��@

- uart_bootloader��z�肵�Ă��܂��B
  
  w.bat ���N������Ə������񂾌�A���s���܂��B
  
- �e��̏������ݕ��@�͉��L�g�o���Q�Ƃ��Ă��������B
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html

- PICKit3�ŏ������ޏꍇ�́Amain32.hex �̂����� pickit3.hex ����������ł��������B



�� ����m�F���@

- LED���_�ł��Ă���ΐ����ł��B

- uart_bootloader���g�p�̏ꍇ�́A���XLED���_�ł��Ă��܂��̂�
  LED�_�ő��x���ς��ΐ����ł��B

- LED�_�łƕ��s���āAUART�̕����G�R�[�o�b�N�����s����Ă��܂��B
 �i�{�[���[�g��uart_bootloader�Ɠ���500kBPS�ł��j

- Pinguino�̃X�P�b�`��A�v���P�[�V�������������ޏꍇ�́A
  uartboot/hostpc/uartflash32.exe -r main32.hex
  �̂悤�ɂ��ď������݂܂��B(-r�I�v�V�����Ŏ��s�܂ōs���܂�)

- BOOT_SW�̓I�[�v����uart bootloader���N�����A�N���[�Y�Ń��[�U�[�v���O����
  ���N������悤�ɂȂ��Ă��܂��B(UBW32�Ƌt�̘_���ɂȂ��Ă��܂�)
  �ʏ�̊J�����́A���Z�b�g�{�^���݂̂ŃX�P�b�`�̍ď������݂��s���܂��B

- ����A�v���P�̋N���Ŏg�p�������ꍇ�́ABOOT_SW�����{�^�����ł͂Ȃ��W�����p�[
  �̂悤�Ȃ��̂ɂ��āA�N���[�Y����Ɨǂ��ł��B


�� �t�@�[���E�F�A�̓������

- UART���瑀�삵�܂����A�R�}���h���C����́APinguino4.X �t���� mphidflash.exe �݊�
 �i�\��j�ł��B

- RESET���邢�͓d���������ARB7�ɐڑ����ꂽBOOT_SW��OFF�ł���� BOOTLOADER���[�h
�@�ɂȂ�܂��B

- BOOTLOADER���[�h�̂Ƃ��� U1RX/U1TX�̃V���A���|�[�g���o�R���āAPC����̃R�}���h
  ��҂��܂��B

- �p�\�R������ uartflash32.exe �����s����ƁA�w�肳�ꂽHEX�t�@�C�����������g��
  9D00_0000�`9D00_7FFF�̗̈�ɏ������񂾌�A0x1000�Ԓn�P�ʂŊJ�n�A�h���X���T�[�`
  ���āA���̊J�n�A�h���X�֕��򂵂܂��B

- ���̂悤�ȓ���Ȃ̂ŁA�Ⴆ��9D00_4000�Ԓn�X�^�[�g��HEX�t�@�C���ł��N�������邱��
  ���\�ł��B
  
- HEX�t�@�C�����S���������܂�Ă��Ȃ���Ԃł�BOOT_SW��ON/OFF�@���ɂ�����炸�A
  BOOTLOADER���[�h�ɂȂ�܂��B

- BOOTLOADER���[�h�̂Ƃ��͓d�����������RB15�Ɍq�����Ă���LED��_�ł��܂��B

- BOOT_SW�������āARESET���邢�͓d�����������ꍇ�́A��������HEX�t�@�C���̃t�@�[��
�@�E�F�A�̂ق����N�����܂��B

���ӁF

  BOOT_SW��(ON/OFF)�_����UBW32�Ƌt�ɂȂ��Ă��܂��B�ύX�������ꍇ�́AHardwareProfile.h��
  #define	Boot_SW_POLARITY	1		// If not PRESSED , Then Run Sketch Program (UBW32 Compat)
  �ɕύX���Ă��������B


�� �g���@�\�ɂ��āB

�i�P�j���[�U�[�G���A��32kB�Ɋg�����Ă��܂��B
	  �܂��A���܂��܂ȊJ�n�Ԓn�̃A�v���������I�ɃT�[�`���܂��B

	�ȉ��̃A�h���X����J�n����A�v���P�[�V�������N���ł��܂��B
	�i�A�h���X�̎Ⴂ������T�[�`�j
      9D00_0000		28k
      9D00_1000		28k
      9D00_2000		24k
      9D00_3000		20k
      9D00_4000		16k

    �A�v���P�[�V�������p�ӂ��銄�荞�݃x�N�^�[�͒ʏ�Ȃ�擪4kB�ɂ���܂��B
    ���̏ꍇ�A���荞�݃x�N�^�[�̃I�t�Z�b�g�O�ɂ͉��������Ȃ�(0xFFFFFFFF)��
    ���邢�́A���Z�b�g�����ւ̕��򖽗߂�u���Ă��������B
    

�i�R�j�ȉ��̃R�}���h��p�ӂ��Ă��܂��B

#define GET_DATA					    0x07	// RAM��PORT����f�[�^��ǂݍ���. �ő�56byte�܂�.
												// 4�̔{���T�C�Y�ł���� int�ŃA�N�Z�X. �����łȂ�
												// �ꍇ��memcpy()���g�p���ăA�N�Z�X.

#define PUT_DATA					    0x09	// GET_DATA�̋t�ŁARAM��PORT�Ƀf�[�^����������.

#define EXEC_CODE					    0x0a	// �w��A�h���X�ɕ��򂷂�.

�R�}���h�̃e�X�g���[�`���� hostpc/ �ȉ��ɂ���܂��B






�� �������[�}�b�v�i�S�́j

PIC32�̃������[�}�b�v�ł��B
- �������蓖�Ă���Ă���G���A�� 0000_0000 �` 2000_0000 ��512MB�ł��B
- �������蓖�Ă���Ă���512MB�ƑS���������̂� KSEG0��KSEG1�ɂ��}�b�v����܂��B
- KSEG0��KSEG1�̈Ⴂ�̓L���b�V������/�L���ŕ������Ă��܂��B

FFFF_FFFF +---------------+
          |               |
          | Reserved      |
          |               |
C000_0000 +---------------+
          | KSEG1(�_��)   | Cache�Ȃ�.
A000_0000 +---------------+
          | KSEG0(�_��)   | Cache����.
8000_0000 +---------------+
          |               |
          | Reserved      |
          |               |
          |               |
          |               |
2000_0000 +---------------+
          | �����������[  | ROM/RAM/PORT
0000_0000 +---------------+



�� �������[�}�b�v�iFlash ROM/RAM�̈�j

A000_1FFF +---------------+
          |               |
          |   SRAM (8kB)  |
          |               |
A000_0000 +---------------+

(BFC00BFF)
9FC0_0BFF +---------------+
          |BOOT Flash(3kB)| RESET����̊J�n�Ԓn��BFC0_0000�ł��B
9FC0_0000 +---------------+ Config Fuse�� BFC0_0BF0�`BFC0_0BFF��16byte�ł��B
(BFC00000)                  ���荞�݃x�N�^�[��BOOT Flash���ɒu����܂��B

9D00_7FFF +---------------+
          |               |
          |Program Flash  |
          |    (32kB)     |
          |               |
9D00_0000 +---------------+



�� �c�[��

- xdump   :  �o�C�i���[�t�@�C�����P�U�i���Ń_���v����.

- hex2dump:  HEX�t�@�C�����P�U�i���_���v���X�g�ɂ���. / HEX�t�@�C���̃R�}���h05���t�B���^�����O����B

- hex2pickit3:  HEX�t�@�C���� PICKit3�ŏ������߂郁�����[��Ԃ�HEX�ɕϊ�����.

- pic32prog.exe : PicKit2���o�R����PIC32MX��HEX�t�@�C������������.




�� �ӎ�

  pic32mx��pic32prog�ɂ��Ẵm�E�n�E�̑�������z����̂g�o�ɂĕ׋������Ă��������܂����B
  �����Ɋ��ӂ̈ӂ�\���܂��B���肪�Ƃ��������܂����B


