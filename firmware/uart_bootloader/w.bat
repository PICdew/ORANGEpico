
rem pic32prog���g����HEX����������.
rem
rem ���ӁF�p�X�ݒ肪�K�v�ł��B
rem   PATH C:\Pinguino-11\compilers\p32\bin;%PATH%

rem ICSP
pic32prog main32.hex

rem BOOTLOADER
rem mphidflash -n -r -w main32.hex
