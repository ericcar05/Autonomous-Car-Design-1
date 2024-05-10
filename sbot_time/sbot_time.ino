#define IN1 29
#define IN2 28
#define ENR 10

#define IN3 30
#define IN4 31
#define ENL 11

void setup()
{
    Serial.begin(9600);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENL, OUTPUT);
    pinMode(ENR, OUTPUT);
}

void motor_l(int speed)
{
    if (speed >= 0)
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(ENL, speed); // 0-255
    }
    else
    {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(ENL, -speed);
    }
}

void motor_r(int speed)
{
    if (speed >= 0)
    {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(ENR, speed); // 0-255
    }
    else
    {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(ENR, -speed);
    }
}

void robot_control(int left_motor_speed, int right_motor_speed)
{
    motor_l(left_motor_speed);
    motor_r(right_motor_speed);
}


void loop()
{
    robot_control(0, 0);
    delay(3000);
    robot_control(60, 50);
    delay(2300);
    robot_control(0, 0);
    delay(300);
    robot_control(255, -255);
    delay(350);            // 회전하는 시간
    robot_control(0, 0);
    delay(300);
    robot_control(130, 100);
    delay(4000);
    robot_control(0, 0);
    delay(300);
    robot_control(255, -255);
    delay(350);            // 회전하는 시간
    robot_control(0, 0);
    delay(300);
    robot_control(130, 100);
    delay(5000);
    robot_control(0, 0);
    delay(5000);
}
