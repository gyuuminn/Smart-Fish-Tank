#include <Servo.h>
#include <SoftwareSerial.h>

#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
SoftwareSerial serial1(A1, A0);// SoftwareSerial Port번호

Servo Sv;
int motor = 5; // 서보 모터 핀 번호
int angle;

int ledPin1 = 7;//LED설정
int ledPin2 = 8;
int ledPin3 = 9;
int readValue = 0;
int lightsensor = A5;//조도센서 설정

void setup() {
  serial1.begin(9600);
  Serial.begin(115200); // 시리얼 통신 시작
  Sv.attach(motor); // 서보 모터 핀 연결
  sensors.begin();  // Start up the library*/
  // 디버깅 메시지 추가
  Serial.println("Setup complete. Waiting for data...");
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  
}
void loop() {
  readValue = analogRead(lightsensor);
  Serial.print("조도: ");
  Serial.print(readValue);
  Serial.println();

  if(readValue < 400){
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
  }else{
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
  }
  // Send the command to get temperatures
  sensors.requestTemperatures(); 
  //print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print(" C");
  Serial.println("");
  
  if (serial1.available() > 0) { // 소프트웨어 시리얼에서 데이터 수신
    String timeInput = serial1.readString(); // 데이터를 문자열로 읽음
    Serial.print("Received time: "); // 디버깅 메시지 추가
    Serial.print(timeInput); // 받은 시간 출력

    // 시간 형식이 올바른지 확인
    if (timeInput.length() >= 8 && timeInput.charAt(2) == ':' && timeInput.charAt(5) == ':') {
      int hour = timeInput.substring(0, 2).toInt();
      int minute = timeInput.substring(3, 5).toInt();
      int second = timeInput.substring(6, 8).toInt();

      // 디버깅 메시지 추가: 파싱된 시간 값 출력
      Serial.print("Parsed time - Hour: ");
      Serial.print(hour);
      Serial.print(", Minute: ");
      Serial.print(minute);
      Serial.print(", Second: ");
      Serial.println(second);

      // 서보 모터 제어
      moveServo(0,0);
      if (hour == 8 && minute == 0 && second == 0) {
        Serial.println("Activating servo motor at 8:0:0"); // 디버깅 메시지 추가
        moveServo(0, 50);
        delay(500);
        moveServo(50, 0);
        Serial.println("Servo motor moved at 8:0:0"); // 디버깅 메시지 추가
      }

      if (hour == 20 && minute == 0 && second == 0) {
        Serial.println("Activating servo motor at 20:00:00"); // 디버깅 메시지 추가
        moveServo(0, 50);
        delay(500);
        moveServo(50, 0);
        Serial.println("Servo motor moved at 20:00:00"); // 디버깅 메시지 추가
      }
    } else {
      Serial.println("Invalid time format received");
    }
  }
}

void moveServo(int startAngle, int endAngle) {
  if (startAngle < endAngle) {
    for (angle = startAngle; angle <= endAngle; angle++) {
      Sv.write(angle);
      delay(15); // 지연 시간을 늘려 모터의 부드러운 움직임을 유도
    }
  } else {
    for (angle = startAngle; angle >= endAngle; angle--) {
      Sv.write(angle);
      delay(15); // 지연 시간을 늘려 모터의 부드러운 움직임을 유도
    }
  }
}
