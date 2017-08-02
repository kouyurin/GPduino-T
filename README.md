GPduino-T
=========

## 概要
GPduino-Tは、ESP32搭載のWiFiラジコン戦車制御ボードです。

以下のことができます。

* DCモータ4個を制御 (最大連続1.2A / 最大瞬間3.0A)
* LED 8個を制御
* サウンド出力
* 赤外線信号(38kHz変調)の送受信
* 電源 3.6V～7.2V (4.8～6V推奨)、電源電圧を監視
* WiFiによる通信　(APにもSTAにもなれる)

また、Waltersons製 1/24 バトルタンクシリーズ互換のIRバトルが可能なファームウェアを提供します。

## ファイル一覧

* hard/GPduino-T/
	* GPduino-T.sch/brd: 回路/基板設計データ(Eagle形式)
	* GPduino-T.pdf: 回路図(PDF)
	* GPduino-T_BOM.xlsx: BOMリスト(Excel形式)
	* GPduino-T.GBL/GBO/GBP/GBS/GML/GTL/GTO/GTP/GTS/TXT: ガーバデータ
* firm/GPduinoT/
	* GPduinoT.ino: ラジコン戦車のArduinoスケッチ
	* *.h, *.cpp: ラジコン戦車のための各種クラス宣言と実装 
* data/
	* cannon.bin: 砲撃音のサウンドデータ
* LICENSE: Apache Licence 2.0です。
	* ただし、回路/基板設計は、Creative Commons Attribution Share-Alike 3.0です。
* README.md これ

## Arduino開発環境のセットアップ
GPduino-T はESP32を搭載しており、Arduino IDEでファームウェアを開発できます。

* まず、ArduinoIDE をインストールしておきます。 (ここでは 1.8.2 を使用)
* 残念ながら、Arduino Core for the ESP32はArduino IDEのボードマネージャには対応しておらず、以下のように手作業でインストールする必要があります。
* まず、以下のリポジトリからクローンします。もしもGitが使えない場合、こちらからDownload ZIPして解凍してもかまいません。アップデートの時の手間だけの問題かと思います。
	* ソース: https://github.com/espressif/arduino-esp32.git
    * 保存先: C:/Users/[YOUR_USER_NAME]/Documents/Arduino/hardware/espressif/esp32
* 次に、下記のバイナリを実行します。
	* C:/Users/[YOUR_USER_NAME]/Documents/Arduino/hardware/espressif/esp32/tools/get.exe
* DOS窓が開いてインストールが実行されます。あとはDOS窓が閉じるまで待ちます。
* ノートン先生をはじめ一部のセキュリティソフトは、いくつかのバイナリをウィルスとして検出してしまうようです。(ノートン先生ではTrojan.Gen.8!cloudとして検出されます。)
* Arduino IDEの[ツール] > [マイコンボード]で[ESP32 Dev Module]を選択します。
* [ツール] > [Upload Speed]で[115200]を選択します。

## Arduinoスケッチの作成

Waltersons製 1/24 バトルタンクシリーズ互換のIRバトルが可能な以下のファームウェアを提供しています。
* firm/GPduinoT/
	* GPduinoT.ino: ラジコン戦車のArduinoスケッチ
	* *.h, *.cpp: ラジコン戦車のための各種クラス宣言と実装

ユーザーが独自にファームウェアを作成する場合も、これを参考にしてください。

ラジコン戦車のための各種クラスを利用するためには、Wire.h と GPduinoR.h をインクルードします。
``` c
#include <Wire.h>
#include "GPduinoR.h"
```
すると、以下のクラスが使用できます。詳細はソースを参照してください。

* Battery: バッテリー電圧の取得
* DcMotor: DCモータの制御
* IR: 赤外線信号の送受信
* Led: LEDの制御
* Solenoid: ソレノイドの制御
* Sound: サウンドの出力
* UdpComm: WiFiによるUDP/IP通信

## Arduinoスケッチの書き込み
* GPduino-TをUSBケーブルでPCに接続します。
* 書き込み時もUSBからの給電は受けません。CN1から給電してください。
* Windows 7以降であればドライバが自動でインストールされ、COMポートとして認識されます。
	* 自動でデバイスが認識されない環境ではFTDIのデバイスドライバをインストールしてください。
* [ツール] > [シリアルポート]でESP32のポートを選択する。
* BOOTボタン(SW2)を押し下げた状態で、RESETボタン(SW1)を押して離します。
* Uploadボタンで、ファームウェアを書き込みます。

## 既知の問題点

* 7.2V電源使用時に発熱が大きい。(3端子レギュレータをDCDCコンバータに置き換えるべき)
* サウンドのボリューム調整のためのトリマを追加すべき。
* サウンド再生時にCPUがクラッシュする場合があるため、クラッシュ回避のごまかし処理を入れている。