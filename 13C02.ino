#include <Servo.h>

#define PIN_SERVO 10   // 서보모터 제어 핀
#define _SERVO_SPEED1 3.0   // 첫 번째 속도 설정
#define _SERVO_SPEED2 0.3   // 두 번째 속도 설정

Servo myServo;

void setup() {
  myServo.attach(PIN_SERVO);
  myServo.write(0); // 초기 위치
  delay(1000);

  slowMove(0, 180, 60);
  delay(2000);

  slowMove(0, 90, 300);
  delay(2000);
}

void loop() {
}
// 서보를 startAngle → endAngle로 ‘durationSec’초 동안 등속 이동
void slowMove(int startAngle, int endAngle, int durationSec) {
  int step = (endAngle > startAngle) ? 1 : -1;
  int angle = startAngle;
  int totalSteps = abs(endAngle - startAngle);
  float delayTime = (durationSec * 1000.0) / totalSteps; // 각도당 지연시간(ms)

  for (int i = 0; i <= totalSteps; i++) {
    myServo.write(angle);
    angle += step;
    delay(delayTime);
  }
}
