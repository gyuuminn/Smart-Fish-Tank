#include <Ds1302.h>

#define PIN_ENA 2                                  // DS1302 RST PIN을 아두이노 2번에 연결    
#define PIN_DAT 3                                  // DS1302 DAT PIN을 아두이노 3번에 연결
#define PIN_CLK 4                                  // DS1302 CLK PIN을 아두이노 4번에 연결

Ds1302 rtc(PIN_ENA, PIN_CLK, PIN_DAT);             // rtc 객체 생성

void setup()
{
  Serial.begin(115200);                            // 시리얼 통신 시작, 보레이트 115200 
  Serial.println("RTC Setting");                   // 시리얼 모니터 "RTC Setting" 출력
  rtc.init();                                      // rtc 객체 초기화
  Ds1302::DateTime dt = {                          // 현재 시간을 넣을 구조체 변수 선언 및 초기 시간 설정
    .year   = 24,                                  // 년도 0 ~ 99
    .month  = 6,                                   // 달   1 ~ 12
    .day    = 17,                                  // 월   1 ~ 32
    .hour   = 17,                                   // 시간 0 ~ 23
    .minute = 15,                                  // 분   0 ~ 59
    .second = 0,                                  // 초   0 ~ 59
    .dow    = 1                                   // 요일 1 ~ 7, 1(월요일)  ~ 7(일요일)
  };
  rtc.setDateTime(&dt);                            // 위 구조체 변수 값을 RTC에 저장하기
}

void loop()
{
}
