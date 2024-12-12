#include <SoftwareSerial.h> //SoftwareSerial 라이브러리 추가
SoftwareSerial serial1(12,11); //송수신 포트번호

#include <Wire.h> //LCD
#include <EEPROM.h> 

#include <Ds1302.h>
#define PIN_ENA 2 // DS1302 RST PIN을 아두이노 2번에 연결
#define PIN_DAT 3 // DS1302 DAT PIN을 아두이노 3번에 연결
#define PIN_CLK 4 // DS1302 CLK PIN을 아두이노 4번에 연결
Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);              // rtc 객체 생성
Ds1302::DateTime now;                               // RTC에서 현재 시간을 가져와 저장할 구조체 변수
const String WeekDays[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
int daysInMonth(int month, int year) {
  switch (month) {
    case 4: case 6: case 9: case 11: return 30;
    case 2: return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    default: return 31;
  }
} //월 일수 계산 및 윤년 계산

#include <LiquidCrystal_I2C.h>//LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); //lcd 초기 설정

unsigned long lastSwitchTime = 0; // 마지막 전환 시간을 저장
uint8_t displayState = 0;
unsigned long lastTempRequestTime = 0;
unsigned long lastSerialSendTime = 0; // 마지막 시리얼 전송 시간을 저장

#include "GravityTDS.h" //TDS
#define TdsSensorPin A2
GravityTDS gravityTds; //Tds 객체 생성
float temperature = 25,tdsValue = 0;

#include<OneWire.h> // 방수온도센서 및 PH센서 사용 DS18B20
#include<DallasTemperature.h>
#define ONE_WIRE_BUS 2// 방수온도센서 핀 번호
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;


#include "DHT.h"
#define DHTPIN 10      // DHT11 센서가 연결된 핀
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

int wlsensor = A1;                           // 수위센서 A0핀 설정
int wlvalue = 0;  // 수위센서 value

void setup()
{
  Serial.begin(115200);                             // 시리얼 통신 시작, 보레이트 115200 
  serial1.begin(9600); // 시리얼1 통신 시작 (두 번째 아두이노와 통신)
  rtc.init();
  lcd.init();
  lcd.begin(16,2); //lcd 객체 초기화
  lcd.backlight();// rtc 객체 초기화

  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on  Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
  dht.begin();
}

void loop()
{
  if (millis() - lastSwitchTime >= 5000) {
    displayState = (displayState+1)%4;
    lastSwitchTime = millis();
    lcd.clear();
  }
  rtc.getDateTime(&now);                            // 구조체 변수에 현재 시간 저장
  if (millis() - lastSerialSendTime >= 1000) { // 1초마다 전송
    if (now.hour < 10) serial1.print("0");
    serial1.print(now.hour);
    serial1.print(":");
    if (now.minute < 10) serial1.print("0");
    serial1.print(now.minute);
    serial1.print(":");
    serial1.println(now.second);
    lastSerialSendTime = millis();
  }
  
  
  if (displayState == 0) { // 창 1
    static uint8_t last_second = -1;                   // 1초를 확인하기 위한 변수

    if (last_second != now.second)                    // 1초마다 '년-월-일 요일 시:분:초'형태로 출력
    {
      last_second = now.second;                       // 현재 초 업데이트
      lcd.setCursor(3, 0);
      lcd.print("20");                             // 21세기 표기
      lcd.print(now.year);                         // 년도 0-99 값
      lcd.print('-');

      if (now.month < 10) lcd.print('0');          // 달이 10미만이면 '0' 출력
      lcd.print(now.month);                        // 달 출력 1-12 값
      lcd.print('-');

      if (now.day < 10) lcd.print('0');            // 일이 10미만이면 '0' 출력
      lcd.print(now.day);     // 01-31             // 일 출력 1~31 값
      lcd.print(' ');
      lcd.setCursor(2, 1);
      lcd.print(WeekDays[now.dow - 1]);            // RTC 저장값이 (1-7)이므로 배열값에 대응하기 위해 -1
      lcd.print(' ');
      if (now.hour < 10) lcd.print('0');           // 시간이 10미만이면 '0' 출력
      lcd.print(now.hour);                         // 시간 출력 0~23 값 
      lcd.print(':');
      if (now.minute < 10) lcd.print('0');         // 분이 10미만이면 '0' 출력
      lcd.print(now.minute);                       // 분 출력 0~59 값
      lcd.print(':');
      if (now.second < 10) lcd.print('0');         // 초가 10미만이면 '0' 출력
      lcd.print(now.second);                       // 초 출력 0~59 값
      }
   } else if(displayState == 1) { //창 2
    lcd.setCursor(0, 0);
    gravityTds.setTemperature(temperature); // 온도 보정 설정
    gravityTds.update(); // 샘플링 및 계산
    tdsValue = gravityTds.getTdsValue(); // TDS 값 가져오기
    lcd.setCursor(0, 0);
    lcd.print("TDS: ");
    lcd.print(tdsValue, 0);
    lcd.print(" ppm");
   } else if(displayState == 2) { //창 3
      //수위센서
    lcd.setCursor(0, 0);
    wlvalue = analogRead(wlsensor);     // 수위센서값을 읽어 변수 value에 저장
    lcd.print("Water Level: ");
    lcd.print(wlvalue);          // 수위 value값 출력
    lcd.setCursor(0,1);
    if (wlvalue>450&&wlvalue<750){
      lcd.print("Normal");
    }else if(wlvalue<450){
      lcd.print("Low");
    }else if (wlvalue>750){
      lcd.print("High");
    }
  } else if(displayState ==3) { //창 4
    //온습도 설정
    int humidity = dht.readHumidity();    // 습도 읽기
    int temperature = dht.readTemperature(); // 온도 읽기
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    lcd.setCursor(0,0);
    lcd.print("Temperature: "); 
    lcd.print(temperature); //온도 출력
    lcd.print(" C");
    lcd.setCursor(0,1);
    lcd.print("Humidity: ");
    lcd.print(humidity); // 습도 출력
    lcd.print(" %");
  }
}
