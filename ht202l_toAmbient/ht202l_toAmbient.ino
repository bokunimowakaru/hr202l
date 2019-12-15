/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。
いかなる被害が発生したとしても当方は一切の責任を負いません。

温湿度センサASONG HR202Lの値を読み取る
＃IoTセンサ用クラウドサービスAmbient送信する

回路図などの情報：
https://bokunimo.net/blog/misc/701/

GitHub(Schematics):
https://github.com/bokunimowakaru/hr202l/

                               Copyright (c) 2019 Wataru KUNINO
                               https://bokunimo.net/
*********************************************************************/
#include <WiFi.h>                           // ESP32用WiFiライブラリ
#include "esp_sleep.h"                      // ESP32用Deep Sleep ライブラリ
#include "Ambient.h"

#define PIN_EN 2                            // 動作確認用LED
#define PIN_HR202_A1  32                    // ASONG HR202L用
#define PIN_HR202_A2  33                    // ASONG HR202L用
#define PIN_HR202_AIN 34                    // ASONG HR202L用

#define SSID "1234ABCD"                     // 無線LANアクセスポイントのSSID
#define PASS "password"                     // パスワード
#define Ambient_channelId 100               // AmbientのチャネルID 
#define Ambient_writeKey "0123456789abcdef" // Ambientのライトキー 
#define SLEEP_P 30*1000000ul                // スリープ時間 30秒(uint32_t)

WiFiClient client;
Ambient ambient;

void setup(){                               // 起動時に一度だけ実行する関数
    int waiting=0;                          // アクセスポイント接続待ち用
    pinMode(PIN_EN,OUTPUT);                 // LED用ポートを出力に
    digitalWrite(PIN_EN,HIGH);              // LED用ポートをONに
    hr202_Setup(PIN_HR202_A1,PIN_HR202_A2,PIN_HR202_AIN);   // 湿度センサ初期化
    
    Serial.begin(115200);                   // 動作確認のためのシリアル出力開始
    Serial.println("ESP32 HR202L HUM");     // 「Example 09」をシリアル出力表示
    WiFi.mode(WIFI_STA);                    // 無線LANをSTAモードに設定
    WiFi.begin(SSID,PASS);                  // 無線LANアクセスポイントへ接続
    while(WiFi.status() != WL_CONNECTED){   // 接続に成功するまで待つ
        delay(100);                         // 待ち時間処理
        waiting++;                          // 待ち時間カウンタを1加算する
        if(waiting%10==0)Serial.print('.'); // 進捗表示
        if(waiting > 300) sleep();          // 300回(30秒)を過ぎたらスリープ
    }
    Serial.println(WiFi.localIP());         // 本機のIPアドレスをシリアル出力
    ambient.begin(Ambient_channelId, Ambient_writeKey, &client);
}

void loop(){
    WiFiUDP udp;                            // UDP通信用のインスタンスを定義
    float temp,hum;                         // センサ用の浮動小数点数型変数

    temp=hr202_getTemp();                   // 温度を取得して変数tempに代入
    hum =hr202_getHum();                    // 湿度を取得して変数humに代入
    if( temp>-100. && hum>=0.){             // 適切な値の時
        ambient.set(1,String(temp).c_str());// 変数tempの値をAmbientへ送信
        Serial.print(temp,2);               // シリアル出力表示
        Serial.print(", ");                 // シリアル出力表示
        ambient.set(2,String(hum).c_str()); // 変数humの値をAmbientへ送信
        Serial.println(hum,2);              // シリアル出力表示
        ambient.send();                     // 送信の終了(実際に送信する)
    }
    sleep();
}

void sleep(){
    digitalWrite(PIN_EN,LOW);               // センサ用の電源をOFFに
    delay(200);                             // 送信待ち時間
    esp_deep_sleep(SLEEP_P);                // Deep Sleepモードへ移行
}
