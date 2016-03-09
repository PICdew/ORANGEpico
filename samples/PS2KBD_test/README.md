

![画像](https://raw.githubusercontent.com/iruka-/ORANGEpico/master/images/NTSC.jpg)

---------------------------------------------------------------------
	PIC32MX250F128B で NTSCグラフィック出力 / PS2キー入力 サンプル.
---------------------------------------------------------------------

■ 概要

*  Pinguino のコンパイラを使用して、NTSCグラフィック出力サンプルをビルドします。

*  PS/2 キーボードから入力された文字をUART1(500kBPS)とNTSC画面にエコーバックします。

*  スタンドアローンで動作するHEXと、Pinguinoのブートローダーから書き込んで
   実行できるHEXを共通化してあります。どちらで書き込んでも動作します。


■ 解像度
*  256x208dotぐらいです.(VRAMは320x208)
*  SPIクロックは6MHzです

■ 配線         PIC32MX250F128B 

                    3.3V
                     |
                     *------10Ω--------------+
                    10k                       |
                     |       ___    ___       | 0.1u
       ラ   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
       イ   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
       ター -->  --PGC3/RA1 [3       26] RB15--1kΩ-------|＞|--GND
              ---HSYNC--RB0 [4       25] RB14
                      --RB1 [5       24] RB13--240Ω------------>Video OUT<---560Ω----RB0(HSYNC/VSYNC)
                      --RB2 [6       23] Vusb3v3--------3.3V
                   SCL2/RB3 [7       22] usb D-
        Xtal     GND----Vss [8       21] usb D+   +10uF
     +-----------------OSC1 [9       20] Vcap------||---GND
     *--|□|--*--------OSC2 [10      19] Vss------------GND
     |  8MHz  |    U1TX/RB4 [11      18] RB9 --- PS2 CLOCK          *--10k------3.3V
     22pF    22pF  U1RX/RA4 [12      17] RB8 --- PS2 DATA           |
     |        |   3.3v--Vdd [13      16] RB7 -----------------------*--BOOT_SW--GND
     |        |         RB5 [14      15] Vbus-----------USB Vbus(5V)
     GND    GND              ~~~~~~~~~~

■ NTSC出力の配線

    Video -- 560 RB0
             240 RB13


■ PS/2キーボードの配線

    18] RB9 --- PS2 CLOCK  
    17] RB8 --- PS2 DATA   

両方とも10kΩにて プルダウンします。

■ UART1の配線

    U1TX/RB4 [11 
    U1RX/RA4 [12 

500k BPSにて、適当なUSB-Serial変換器(3.3v TTL信号)にTxD/RxDクロスで繋ぎます。

            
■ Pinguino 開発環境構築
            
- Windows XP / Vista / 7 / 8.1 / 10 / Ubuntu Linux のどれかを用意します。(64bit OK)

- Pinguino-11 を下記サイトから入手してインストールします。
        http://www.pinguino.cc/download.php


■ MPLAB 開発環境の追加
            
- 不要になりました。


■ コンパイル方法

- コマンドラインから 

  D:>  make

  でビルドしてください。

■ コンパイル上の注意点

- Windows / Linux は Make変数で自動判別します。


■ ファームウェアHEXの書き込み方法

* uart_bootloaderを想定しています。
  
  w.bat を起動すると書き込んだ後、実行します。
  
* 各種の書き込み方法は下記ＨＰを参照してください。
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html

* PICKit3で書き込む場合は、main32.hex のかわりに pickit3.hex を書き込んでください。


■ ファームウェアの動作説明

* NTSC信号を出力します。

* 簡易なグラフ命令を実行しています。

■ メモリーマップ（全体）

PIC32のメモリーマップです。
* 物理割り当てされているエリアは 0000_0000 ～ 2000_0000 の512MBです。
* 物理割り当てされている512MBと全く同じものが KSEG0とKSEG1にもマップされます。
* KSEG0とKSEG1の違いはキャッシュ無効/有効で分けられています。

 

    FFFF_FFFF +---------------+
              |               |
              | Reserved      |
              |               |
    C000_0000 +---------------+
              | KSEG1(論理)   | Cacheなし.
    A000_0000 +---------------+
              | KSEG0(論理)   | Cacheあり.
    8000_0000 +---------------+
              |               |
              | Reserved      |
              |               |
              |               |
              |               |
    2000_0000 +---------------+
              | 物理メモリー  | ROM/RAM/PORT
    0000_0000 +---------------+
    
    

■ メモリーマップ（Flash ROM/RAM領域）

    A000_7FFF +---------------+
              |               |
              |   SRAM (32kB) |
              |               |
    A000_0000 +---------------+
    
    (BFC00BFF)
    9FC0_0BFF +---------------+
              |BOOT Flash(3kB)| RESET直後の開始番地はBFC0_0000です。
    9FC0_0000 +---------------+ Config Fuseは BFC0_0BF0～BFC0_0BFFの16byteです。
    (BFC00000)                  割り込みベクターもBOOT Flash内に置かれます。
    
    9D00_1FFFF+---------------+
              |               |
              |Program Flash  |
              |    (128kB)    |
              |               |
    9D00_0000 +---------------+


■ 謝辞

  pic32mxやpic32progについてのノウハウの多くをすzさんのＨＰにて勉強させていただきました。
  ここに感謝の意を表します。ありがとうございました。


