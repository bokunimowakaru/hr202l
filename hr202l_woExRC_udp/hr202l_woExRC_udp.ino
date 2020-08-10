/*******************************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。
いかなる被害が発生したとしても当方は一切の責任を負いません。

温湿度センサ AOSONG HR202L の目安値を読み取る 【外部ＲＣ部品不要・簡易版】
＋UDPでブロードキャスト送信する

※ センサ本来の性能は発揮できません。
※ 精度がとても悪いので、ご注意ください。

This simplified software does NOT give you adequate accuracy.

回路図などの情報：
https://bokunimo.net/blog/esp/903/

AOSONG HR202L without External RC Components.
Schematics: Connect each of HR202L pins to the ESP32 GPIO 32 and 33.

                               Copyright (c) 2019-2020 Wataru KUNINO
                               https://bokunimo.net/
*******************************************************************************/

#include <WiFi.h>                           // ESP32用WiFiライブラリ
#include <WiFiUdp.h>                        // UDP通信を行うライブラリ
#include "esp_sleep.h"                      // ESP32用Deep Sleep ライブラリ

#define PIN_EN 2                            // 動作確認用LED
#define PIN_HR202_A1 32                     // ASONG HR202L用
#define PIN_HR202_A2 33                     // ASONG HR202L用

#define SSID "1234ABCD"                     // 無線LANアクセスポイントのSSID
#define PASS "password"                     // パスワード
#define SENDTO "255.255.255.255"            // 送信先のIPアドレス
#define PORT 1024                           // 送信のポート番号
#define SLEEP_P 30*1000000ul                // スリープ時間 30秒(uint32_t)
#define DEVICE "humid_1,"                   // デバイス名(5文字+"_"+番号+",")
#define TEMP_ADJ -15.0                      // 温度値の補正用

void setup(){                               // 起動時に一度だけ実行する関数
    int waiting=0;                          // アクセスポイント接続待ち用
    pinMode(PIN_EN,OUTPUT);                 // LED用ポートを出力に
    digitalWrite(PIN_EN,HIGH);              // LED用ポートをONに
    hr202_Setup(PIN_HR202_A1,PIN_HR202_A2); // 湿度センサの初期化
    hr202_setTempOffset(TEMP_ADJ);          // 温度補正値を設定

    Serial.begin(115200);                   // 動作確認のためのシリアル出力開始
    Serial.println("ESP32 HR202L HUM");     // 「ESP32 HR202L HUM」を出力
    delay(100);                             // 電源の安定化待ち時間
    WiFi.mode(WIFI_STA);                    // 無線LANをSTAモードに設定
    WiFi.begin(SSID,PASS);                  // 無線LANアクセスポイントへ接続
    while(WiFi.status() != WL_CONNECTED){   // 接続に成功するまで待つ
        delay(100);                         // 待ち時間処理
        waiting++;                          // 待ち時間カウンタを1加算する
        if(waiting%10==0)Serial.print('.'); // 進捗表示
        if(waiting > 300) sleep();          // 300回(30秒)を過ぎたらスリープ
    }
    Serial.println(WiFi.localIP());         // 本機のIPアドレスをシリアル出力
}

void loop(){
    WiFiUDP udp;                            // UDP通信用のインスタンスを定義
    float temp,hum;                         // センサ用の浮動小数点数型変数
    
    temp = hr202_getTemp();                 // 温度を取得して変数tempに代入
    hum  = hr202_getHum();                  // 湿度を取得して変数humに代入
    if( temp>-100. && hum>=0.){             // 適切な値の時
        udp.beginPacket(SENDTO, PORT);      // UDP送信先を設定
        udp.print(DEVICE);                  // デバイス名を送信
        udp.print(temp,1);                  // 変数tempの値を送信
        Serial.print(temp,2);               // シリアル出力表示
        udp.print(", ");                    // 「,」カンマを送信
        Serial.print(", ");                 // シリアル出力表示
        udp.println(hum,1);                 // 変数humの値を送信
        Serial.println(hum,2);              // シリアル出力表示
        udp.endPacket();                    // UDP送信の終了(実際に送信する)
    }
    sleep();
}

void sleep(){
    digitalWrite(PIN_EN,LOW);               // LED用ポートをOFFに
    delay(200);                             // 送信待ち時間
    esp_deep_sleep(SLEEP_P);                // Deep Sleepモードへ移行
}
