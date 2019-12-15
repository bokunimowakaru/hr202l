/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
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
*********************************************************************/

#define TEMP_ADJ 33            // 内蔵温度センサの値 オフセット
#define HUMI_ADJ 4             // 湿度センサの値 オフセット(0～8程度)

#define AC_T1 250 -3 +23       // AC波形用 OFF期間
#define AC_T2 250 -3 -23       // AC波形用 ON期間
#define AC_T3 250 -13 -23      // AC波形用 ON期間 - ADC期間

uint8_t _hr202_PIN_A1  = 32;   // IO32に抵抗10kを接続。HR202Lを直列に
uint8_t _hr202_PIN_A2  = 33;   // HR202Lの反対側の端子を接続
uint8_t _hr202_PIN_AIN = 34;   // 抵抗とHR202Lの接続点を入力

float hr202_getTemp(){
    return temperatureRead() - (float)TEMP_ADJ;
}

uint16_t hr202_getVal(){  // 12bit + 4bit(16倍) = 16bit
    uint16_t val=0;
    uint32_t tus0,tus1,tus2,tus3,tus4;
    
    tus0 = micros();
    for(int t=0;t<16;t++){
        delayMicroseconds(AC_T1);
        digitalWrite(_hr202_PIN_A2,LOW);
        tus1 = micros() - tus0;        
        delayMicroseconds(AC_T2);
        digitalWrite(_hr202_PIN_A1,LOW);
        tus2 = micros() - tus0;        
        delayMicroseconds(AC_T1);
        digitalWrite(_hr202_PIN_A2,HIGH);
        tus3 = micros() - tus0;
        val += analogRead(_hr202_PIN_AIN);
        delayMicroseconds(AC_T3);
        digitalWrite(_hr202_PIN_A1,HIGH);
        tus4 = micros() - tus0;
        tus0 = micros();
    }
//  Serial.printf("HR202L t1=%d, t2=%d, t3=%d, t4=%d, \n",tus1,tus2,tus3,tus4);
    return val;
}

float hr202_getHum(){
    for(int i=0;i<62;i++) hr202_getVal();
    uint16_t val2 = hr202_getVal();
    float volt = (float)val2 / 16. / 4095. * 3.5;
    float imp = 33. / volt -10.;
    float i = 10 * log10f( imp );
    float hum0 = -5.897e-4 * powf(i,3) + 6.716e-2 * powf(i,2) - 4.019 * i + 1.187e2;
    float hum = hum0 - hr202_getTemp() / 2.15 +(float)HUMI_ADJ;
//  Serial.printf("HR202L volt=%.1f[V], imp=%.1f[kOhm], ",volt,imp);
//  Serial.printf("log=%.1f[dBkOhm], hum0=%.1f[%%], hum=%.1f[%%]\n",2*imp,hum0,hum);
    return hum;
}

void hr202_Setup(){
    pinMode(_hr202_PIN_A1,OUTPUT);
    pinMode(_hr202_PIN_A2,OUTPUT);
    digitalWrite(_hr202_PIN_A1,HIGH);
    digitalWrite(_hr202_PIN_A2,HIGH);
    pinMode(_hr202_PIN_AIN,INPUT);
    analogSetPinAttenuation(_hr202_PIN_AIN,ADC_11db);
    for(int t=0;t<10;t++) delay(1);
}

void hr202_Setup(uint8_t PIN_A1, uint8_t PIN_A2, uint8_t PIN_AIN){
    _hr202_PIN_A1=PIN_A1;
    _hr202_PIN_A2=PIN_A2;
    _hr202_PIN_AIN=PIN_AIN;
    hr202_Setup();
}

void setup(){
    Serial.begin(115200);
    hr202_Setup();
}

void loop(){
    float temp = hr202_getTemp();
    float hum = hr202_getHum();
    Serial.print("Temp = ");
    Serial.print(temp,1);
    Serial.print(", Humi = ");
    Serial.println(hum,1);
    delay(5000);
}
