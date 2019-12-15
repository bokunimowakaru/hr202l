# AOSONG HR202L で湿度測定し、クラウドサービス Ambient へ送信する

60円から200円程度で販売されている、とても安価な湿度センサ。扱いにくいせいか、あまり製作記事が見当たらなかったので、この湿度センサを使って湿度を測って、Ambientへ送信するサンプルを製作してみました。

## 製作方法
- 下記のブログを参照ください
	- https://bokunimo.net/blog/misc/701/

## 書き込み方法
Arduino IDEで ht202l_toAmbient.ino を開き、下記の項目を修正し、ESP32マイコンに書き込んでください。Ambient_channelIdとAmbient_channelIdは、アンビエントデーター株式会社が運営する[クラウドサービス Ambient](https://ambidata.io/)で取得してください。

	#define SSID "1234ABCD"                     // 無線LANアクセスポイントのSSID
	#define PASS "password"                     // パスワード
	#define Ambient_channelId 100               // AmbientのチャネルID 
	#define Ambient_channelId "0123456789abcdef" // Ambientのライトキー 

## ESP用 Ambient ライブラリについて
本フォルダに含まれる以下のファイルの権利はアンビエントデーター株式会社に帰属します。  
詳細は[README_Ambient.md](README_Ambient.md)をご覧ください。

- Ambient.cpp
	- ambient.cpp - Library for sending data to Ambient
	- Created by Takehiko Shimojima, April 21, 2016
- Ambient.h
	- ambient.h - Library for sending data to Ambient
	- Created by Takehiko Shimojima, April 21, 2016

- README_Ambient.md
	- https://github.com/AmbientDataInc/Ambient_ESP8266_lib
	- Created by Takehiko Shimojima, modified on 29 Apr 2018

## HR202L用サンプルについて
その他のファイルについてはライセンスフリーです。  
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。  
いかなる被害が発生したとしても当方は一切の責任を負いません。  

I2C接続の温湿度センサの値を読み取る
ASONG HR202L

回路図などの情報：
https://bokunimo.net/blog/misc/701/

GitHub(Schematics):
https://github.com/bokunimowakaru/hr202l/

                               Copyright (c) 2019 Wataru KUNINO
                               https://bokunimo.net/
