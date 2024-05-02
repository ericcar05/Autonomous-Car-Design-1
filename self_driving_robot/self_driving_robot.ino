#define IN1 29 // 핀 번호는 자신의 환경에 맞도록 설정
#define IN2 28
#define ENR 10
#define IN3 30
#define IN4 31
#define ENL 11

void setup()
{
    pinMode (IN1, OUTPUT);
    pinMode (IN2, OUTPUT);
    pinMode (IN3, OUTPUT);
    pinMode (IN4, OUTPUT);
    pinMode (ENL, OUTPUT);
    pinMode (ENR, OUTPUT);
}

void motor_l(int speed)
{
  if (speed >=0)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite (ENL, speed); // 0-255
  }
  else
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite (ENL, -speed);
  }
}

void motor_r(int speed)
{
  if (speed >=0)
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite (ENR, speed); // 0-255
  }
  else
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite (ENR, -speed);
  }
}

void loop()
{
    motor_l(-30);
    motor_r(40);
}
