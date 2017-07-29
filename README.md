GPduino-T
=========

## 概要
GPduino-Tは、ESP32搭載のWiFiラジコン戦車制御ボードです。  
（工事中）

## ファイル一覧

* hard/GPduino-T/
	* GPduino-T.sch/brd: 回路/基板設計データ(Eagle形式)
	* GPduino-T.pdf: 回路図(PDF)
	* GPduino-T_BOM.xlsx: BOMリスト(Excel形式)
	* GPduino-T.GBL/GBO/GBP/GBS/GML/GTL/GTO/GTP/GTS/TXT: ガーバデータ
* LICENSE: Apache Licence 2.0です。
	* ただし、回路/基板設計は、Creative Commons Attribution Share-Alike 3.0です。
* README.md これ

## ファームウェアの書き込み
GPduino-T はESP32を搭載しており、Arduino IDEでファームウェアを開発できます。

* GPduino-TをUSBケーブルでPCに接続します。
* 書き込み時もUSBからの給電は受けません。CN1から給電してください。
* Windows 7以降であればドライバが自動でインストールされ、COMポートとして認識されます。
	* 自動でデバイスが認識されない環境ではFTDIのデバイスドライバをインストールしてください。
* まず、ArduinoIDE をインストールしておきます。 (ここでは 1.8.2 を使用)
* 残念ながら、Arduino Core for the ESP32はArduino IDEのボードマネージャには対応しておらず、以下のように手作業でインストールする必要があります。
* まず、以下のリポジトリからクローンします。もしもGitが使えない場合、こちらからDownload ZIPして解凍してもかまいません。アップデートの時の手間だけの問題かと思います。
	* ソース: https://github.com/espressif/arduino-esp32.git
    * 保存先: C:/Users/[YOUR_USER_NAME]/Documents/Arduino/hardware/espressif/esp32
* 次に、下記のバイナリを実行します。
	* C:/Users/[YOUR_USER_NAME]/Documents/Arduino/hardware/espressif/esp32/tools/get.exe
* DOS窓が開いてインストールが実行されます。あとはDOS窓が閉じるまで待ちます。
* ノートン先生をはじめ一部のセキュリティソフトは、いくつかのバイナリをウィルスとして検出してしまうようです。(ノートン先生ではTrojan.Gen.8!cloudとして検出されます。)
* Arduino IDEの[ツール] > [マイコンボード]で[ESP32 Dev Module]を選択する。
* [ツール] > [Upload Speed]で[115200]を選択する。
* [ツール] > [シリアルポート]でESP32のポートを選択する。
* BOOTボタン(SW2)を押し下げた状態で、RESETボタン(SW1)を押して離します。
* Uploadボタンで、ファームウェアを書き込みます。


