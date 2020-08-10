/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。
いかなる被害が発生したとしても当方は一切の責任を負いません。

温湿度センサ AOSONG HR202L の目安値を取得【外部ＲＣ部品不要・簡易版】

※ センサ本来の性能は発揮できません。
※ 精度がとても悪いので、ご注意ください。

This simplified software does NOT give you adequate accuracy.

回路図などの情報：
https://bokunimo.net/blog/esp/903/

AOSONG HR202L without External RC Components.
Schematics: Connect each of HR202L pins to the ESP32 GPIO 32 and 33.

                               Copyright (c) 2019-2020 Wataru KUNINO
                               https://bokunimo.net/bokunimowakaru/
*********************************************************************/

float _hr202_temp_offset = -25.0;

uint8_t _hr202_PIN_A1;
uint8_t _hr202_PIN_A2;

boolean hr202_waitlow(){
    pinMode(_hr202_PIN_A1,OUTPUT);
    digitalWrite(_hr202_PIN_A1,LOW);
    pinMode(_hr202_PIN_A2,OUTPUT);
    digitalWrite(_hr202_PIN_A2,LOW);
    for(int t=0;t<50;t++) delay(1);
    return 0;   // PASSED
}

float hr202_print(int a){
    float v = (float)a / 4095. * 1.1;
    Serial.print("ADC (ATT=0; 0dB) = ");
    Serial.print(v,3);
    Serial.print(" [V], ");
    float r = v / (3.3-v) * 35.; 
    Serial.print(r,3);
    Serial.println(" [kOhm]");
    return r;
}

uint16_t hr202_getVal(int us){
    uint16_t val=0;
    uint16_t a1,a2;
    uint32_t t1,t2;
    uint32_t tus0,tus;
    
    for(int t=0;t<8;t++){
        // SETUP T1
        if( hr202_waitlow() ) return 65535;
        analogSetPinAttenuation(_hr202_PIN_A1,ADC_0db);
        
        // START T1
        tus0 = micros();
        pinMode(_hr202_PIN_A1,INPUT_PULLUP);
        delayMicroseconds(us);  //      1 / 1k Hz / 2 / 6.6 V = 75 us(最大)
        a1 = analogRead(_hr202_PIN_A1);
        tus = micros();
        pinMode(_hr202_PIN_A1,OUTPUT);
        digitalWrite(_hr202_PIN_A1,LOW);
        t1 = tus - tus0;
    //  hr202_print(a1);
        
        // SETUP T2
        if( hr202_waitlow() ) return 65535;
        analogSetPinAttenuation(_hr202_PIN_A2,ADC_0db);
        
        // START T2
        tus0 = micros();
        pinMode(_hr202_PIN_A2,INPUT_PULLUP);
        delayMicroseconds(us);  //      1 / 1k Hz / 2 / 6.6 V = 75 us(最大)
        a2 = analogRead(_hr202_PIN_A2);
        tus = micros();
        pinMode(_hr202_PIN_A2,OUTPUT);
        digitalWrite(_hr202_PIN_A2,LOW);
        t2 = tus - tus0;
    //  hr202_print(a2);
        
        pinMode(_hr202_PIN_A1,INPUT_PULLDOWN);
        pinMode(_hr202_PIN_A2,INPUT_PULLDOWN);
        
        val += a1 + a2;
    //  Serial.printf("DEBUG HR202L a1=%d, a2=%d, t1=%d, t2=%d, ret=%d\n",a1,a2,t1,t2,val);
    }
    return val;
}

void hr202_setTempOffset(float in){
    _hr202_temp_offset = in;
    return;
}

float hr202_getTemp(){
    return temperatureRead() + _hr202_temp_offset;
}

float hr202_getHum(){
    int avn = 0;
    float hum_avr = 0.;
    for(int i=0; i < 8; i++){
        float temp = hr202_getTemp();
        int hr202_val =(int)hr202_getVal(39);   // センサ値を取得
        float hum;
        if(hr202_val <= 0 || hr202_val >= 65500){
            hum = - (10. - temp / 2.15);
        }else if(hr202_val < 4096){
            hum = -2.999442e-3 * hr202_val + 83.0;
            avn++;
        }else if(hr202_val < 5495){
            hum = -2.972147e-3 * hr202_val + 75.0;
            avn++;
        }else if(hr202_val < 19152){
            hum = -4.241396e-4 * hr202_val + 61.0;
            avn++;
        }else{
            hum = -1.525879e-4 * hr202_val + 55.8;
            avn++;
        }
        /* 温度依存性の補正 */
        hum += 10. - temp / 2.15;
        hum_avr += hum;
        delay(1);
    //  Serial.printf("%d, %d, %f, (%f)\n",i,hr202_val,hum,hum_avr /(float)avn);
    }
    if(avn > 0) return hum_avr / avn;
    return 0.0;
}

void hr202_Setup(uint8_t PIN_A1, uint8_t PIN_A2){
    _hr202_PIN_A1=PIN_A1;
    _hr202_PIN_A2=PIN_A2;
    pinMode(_hr202_PIN_A1,INPUT_PULLDOWN);
    pinMode(_hr202_PIN_A2,INPUT_PULLDOWN);
    analogSetPinAttenuation(_hr202_PIN_A1,ADC_0db);
    analogSetPinAttenuation(_hr202_PIN_A2,ADC_0db);
    for(int t=0;t<10;t++) delay(1);
}
