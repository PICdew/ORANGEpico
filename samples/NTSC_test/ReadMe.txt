---------------------------------------------------------------------
	PIC32MX250F128B �� NTSC�O���t�B�b�N�o�� �T���v��.
---------------------------------------------------------------------

�� �T�v

-  Pinguino �̃R���p�C�����g�p���āANTSC�O���t�B�b�N�o�̓T���v�����r���h���܂��B
-  �X�^���h�A���[���œ��삷��HEX�ƁAPinguino�̃u�[�g���[�_�[���珑�������
   ���s�ł���HEX�����ʉ����Ă���܂��B�ǂ���ŏ�������ł����삵�܂��B


�� �𑜓x
-  240x200dot���炢�ł�.(VRAM��256x200)

-  SPI�N���b�N��4.8MHz�ł�
-  ����ɁA32bit SPI���[�h�Ȃ̂ŁAdot��32�̔{���ɂȂ�܂�

�� NTSC�o�͂̔z��
   Video -- 560 RB0
            240 RB13


�� �z��         PIC32MX250F128B 

                3.3V
                 |
                 *------10��--------------+
                10k                       |
                 |       ___    ___       | 0.1u
   ��   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
   �C   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
   �^�[ -->  --PGC3/RA1 [3       26] RB15--1k��-------|��|--GND
          ---HSYNC--RB0 [4       25] RB14
                  --RB1 [5       24] RB13--240��------------>Video OUT<---560��----RB0(HSYNC/VSYNC)
                  --RB2 [6       23] Vusb3v3--------3.3V
               SCL2/RB3 [7       22] usb D-
    Xtal     GND----Vss [8       21] usb D+   +10uF
 +-----------------OSC1 [9       20] Vcap------||---GND
 *--|��|--*--------OSC2 [10      19] Vss------------GND
 |  8MHz  |    U1TX/RB4 [11      18] RB9
 22pF    22pF  U1RX/RA4 [12      17] RB8
 |        |   3.3v--Vdd [13      16] RB7
 |        |         RB5 [14      15] Vbus-----------USB Vbus(5V)
 GND    GND              ~~~~~~~~~~
            



            
�� Pinguino �J�����\�z
            
- Windows XP / Vista / 7 / 8 �̂ǂꂩ��p�ӂ��܂��B

- Pinguino X.3 ����ł����L�T�C�g������肵�ăC���X�g�[�����܂��B
  http://wiki.pinguino.cc/index.php/Main_Page/ja

- Pinguino X.3 �R���p�C���[�Ƀp�X��ʂ��܂��B

  setenv.bat
    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%


�� MPLAB �J�����̒ǉ�
            
- MPLAB_IDE_8_89.zip ����肵�ăC���X�g�[�����܂��B
  �C���X�g�[�����̑I���ŁAPIC32�p�̃R���p�C���������ɃC���X�g�[������悤�ɂ��Ă��������B


�� Microchip USB���C�u�����̒ǉ�
            
- ����Microchip Libraries for Applications v2012-10-15 Windows ����肵�ăC���X�g�[�����܂��B

  microchip-application-libraries-v2012-10-15-windows-installer.exe

- ���̃A�[�J�C�u��W�J�����f�B���N�g���ƕ���ɁAMicroChip��USB�t���[�����[�N������悤�Ƀf�B���N�g����z�u���܂��B

��j  D:\Pic32\MicroChip\USB\     ��Microchip Libraries for Applications 
      D:\Pic32\pic32mon\          �����̃A�[�J�C�u��W�J��������.


�� �R���p�C�����@

- �R�}���h���C������ 

  D:>  make

  �Ńr���h���Ă��������B


�� �R���p�C����̒��ӓ_

- Pinguino�ɕt����make.exe ���g�p���Ă��������B

    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%

- �����Ŏg�p����Makefile��Cygwin��MinGW��shell�Ɉˑ����Ȃ� (cmd.exe���Ăяo��) make
- �łȂ��Ɛ����������Ȃ��悤�ł��B


�� �������ݕ��@

- pic32prog��z�肵�Ă��܂��B
  http://code.google.com/p/pic32prog/

  pic32prog.exe���p�X�̒ʂ����ꏊ�ɐݒu���Ă���ꍇ��
  wp.bat ���N������Ə������߂܂��B
  
  �������߂����ǂ����m���߂�ꍇ�́A
  r.bat �����s���Ă݂Ă��������B


- �e��̏������ݕ��@�͉��L�g�o���Q�Ƃ��Ă��������B
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html


�� Pinguino�̃u�[�g���[�_�[����̏������ݕ��@

- w.bat �����s���܂��B


���ӁF
	���[�U�[�G���A���L�����邽�߂ɁA�v���O�����J�n�Ԓn��9D00_1000�ɌJ��グ�Ă��܂��B
	�u�[�g���[�_�[�ɂ� HIDBoot_mips32gcc.X.zip �ɓ�������Ă���8KB(+3kB)�̃T�C�Y��
	���̂��g�p���Ă��������B


�� �t�@�[���E�F�A�̓������

- VGA(���m�ɂ�SVGA 800x600 @ 60Hz)�̐M�����o�͂��܂��B
- USB�ڑ����Ē��ڃ|�[�g�������菑���������胁�����[��`�����肷�邱�Ƃ��ł��܂��B
  pic32mon.exe ���g�p���Ă��������B

- �O���t�B�b�NRAM�̊��蓖�Ă�output.map(�R���p�C�����ɐ�������܂�)�ŎQ�Ƃł��܂��B
  �����ύX���Ȃ���΂��Ԃ� 0xa0000004 �Ԓn���� 0x1900 �o�C�g����܂��B
  ���̃������[��pic32mon.exe���珑�������邱�ƂŁA�O���t�B�b�N�\���̃p�^�[����
  ���������ł��܂��B

- �O���t�B�b�N�`��֐���R�}���h�ɂ��Ă͖������ł��B



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

A000_7FFF +---------------+
          |               |
          |   SRAM (32kB) |
          |               |
A000_0000 +---------------+

(BFC00BFF)
9FC0_0BFF +---------------+
          |BOOT Flash(3kB)| RESET����̊J�n�Ԓn��BFC0_0000�ł��B
9FC0_0000 +---------------+ Config Fuse�� BFC0_0BF0�`BFC0_0BFF��16byte�ł��B
(BFC00000)                  ���荞�݃x�N�^�[��BOOT Flash���ɒu����܂��B

9D00_1FFFF+---------------+
          |               |
          |Program Flash  |
          |    (128kB)    |
          |               |
9D00_0000 +---------------+





�� �ӎ�

  pic32mx��pic32prog�ɂ��Ẵm�E�n�E�̑�������z����̂g�o�ɂĕ׋������Ă��������܂����B
  �����Ɋ��ӂ̈ӂ�\���܂��B���肪�Ƃ��������܂����B


