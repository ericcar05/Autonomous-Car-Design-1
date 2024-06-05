#include <NewPing.h>
#include <Wire.h>
#include <LSM303.h>

#define THRESHOLD_ANGLE1 15
#define THRESHOLD_ANGLE2 7


#define LINE_DETECT_WHITE  1

#define motor_speed_offset 40

#define FTRIGGER_PIN 14
#define FECHO_PIN 15
#define LTRIGGER_PIN 17
#define LECHO_PIN 16
#define RTRIGGER_PIN 19
#define RECHO_PIN 18
#define MAX_DISTANCE 100
#define SONAR_NUM 3

#define IN1 29
#define IN2 28
#define ENR 10

#define IN3 30
#define IN4 31
#define ENL 11

LSM303 compass;

int linesensor_data[5] = {0,0,0,0,0};  // 읽은 값을 저장할 변수
int linesensor_pin[5] = {2,3,4,5,6};   // int형 배열
int sum;

NewPing sonar_front(FTRIGGER_PIN, FECHO_PIN, MAX_DISTANCE);
NewPing sonar_left(LTRIGGER_PIN, LECHO_PIN, MAX_DISTANCE);
NewPing sonar_right(RTRIGGER_PIN, RECHO_PIN, MAX_DISTANCE);

float heading_angle = 0.0;
float in_heading_angle = 17.0;
float target_heading_angle = 90;
float heading_angle_error = 0.0;

const int numReadings = 2;
float distance = 0.0;
float ldistance = 0.0;
float rdistance = 0.0;
int freadings[numReadings];  // 신호값을 읽는 배열을 지정. 배열의 크기는 위의 값으로 정함
int lreadings[numReadings];
int rreadings[numReadings];
int readIndex = 0;                 // 몇번째 신호인지를 표시하는 인덱스 변수 
float ftotal = 0;                        // 합계 변수
float ltotal = 0;
float rtotal = 0;
float faverage = 0;                    // 평균값 변수 
float laverage = 0;
float raverage = 0;
float UltrasonicSensorData[SONAR_NUM];
int mode=-1;

void read_ultrasonic_sensor(void)
{
  float distance = sonar_front.ping_cm();  // 센서입력값을 읽어온다
  float ldistance = sonar_left.ping_cm();
  float rdistance = sonar_right.ping_cm();
  ftotal -= freadings[readIndex];      // 가장 오래된 data를 합계에서 빼서 없앤다
  ltotal -= lreadings[readIndex];
  rtotal -= rreadings[readIndex];
  freadings[readIndex] = distance;
  lreadings[readIndex] = ldistance;
  rreadings[readIndex] = rdistance;// 센서입력값을 배열에 저장
  ftotal += freadings[readIndex];     // 읽은 값을 합계에 더한다
  ltotal += lreadings[readIndex];
  rtotal += rreadings[readIndex];
  readIndex++;                  // 신호값을 읽은 인덱스를 1 증가 시킨다.
  if (readIndex >= numReadings)
  {     
    readIndex = 0;                              // 0으로 만들어 처음부터 다시 시작한다
  }
  faverage = ftotal / numReadings;    // 평균값을 구한다
  laverage = ltotal / numReadings;
  raverage = rtotal / numReadings;
  UltrasonicSensorData[0]=faverage;
  UltrasonicSensorData[1]=laverage;
  UltrasonicSensorData[2]=raverage;
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  int i;
  Wire.begin();
  compass.init();
  compass.enableDefault();
  
  compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENL, OUTPUT);
  pinMode(ENR, OUTPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  { 
    freadings[thisReading] = 0;
    lreadings[thisReading] = 0;
    rreadings[thisReading] = 0;
  }
  for(i=0;i<5;i++)
  {
    pinMode(linesensor_pin[i],INPUT);
  }
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

int read_digital_line_sensor(void)
{
  int i;
  sum=0;
  for(i=0;i<5;i++)
  {
    if(LINE_DETECT_WHITE == 0)
    {
      linesensor_data[i] = 1 - digitalRead(linesensor_pin[i]);
    }
    else
    {
      linesensor_data[i] = digitalRead(linesensor_pin[i]);
    }
    sum += linesensor_data[i];
  }

  if(sum == 5)
  {
    return sum;
  }
  else if(sum == 2)
  {
    if( (linesensor_data[3] == 1) && (linesensor_data[4] == 1) ) return 3;
    if( (linesensor_data[2] == 1) && (linesensor_data[3] == 1) ) return 1;
    if( (linesensor_data[1] == 1) && (linesensor_data[2] == 1) ) return -1;
    if( (linesensor_data[0] == 1) && (linesensor_data[1] == 1) ) return -3;
  }
  else if(sum == 1)
  {
    if((linesensor_data[0] == 1)) return -4;
    if((linesensor_data[1] == 1)) return -2;
    if((linesensor_data[2] == 1)) return 0;
    if((linesensor_data[3] == 1)) return 2;
    if((linesensor_data[4] == 1)) return 4;
  }
  else if(sum == 3)
  {
    return -10;
  }
  else
  {
    return -5;
  }
}

void imu_rotation(void)
{
  bool flag=1;
  while(flag)
  {
    compass.read();
    float heading1 = compass.heading();
    compass.read();
    float heading2 = compass.heading();
    float heading = (heading1 + heading2)/2;

    heading = 360 - heading;

    Serial.println(heading);

    heading_angle_error = target_heading_angle - heading;

    if(heading_angle_error > 180)
    {
      heading_angle_error = heading_angle_error - 360;
    }
    else if(heading_angle_error < -180)
    {
      heading_angle_error = heading_angle_error + 360;
    }

    
    if(heading_angle_error > THRESHOLD_ANGLE1)
    {
      robot_control(-200,200);
    }
    else if((heading_angle_error >= -THRESHOLD_ANGLE2)&&(heading_angle_error <= THRESHOLD_ANGLE1))
    {
      robot_control(-30,30);
    }
    else if((heading_angle_error >= -THRESHOLD_ANGLE2)&&(heading_angle_error <= THRESHOLD_ANGLE2))
    {
      robot_control(0,0);
      flag=0;
    }
    else if((heading_angle_error >= -THRESHOLD_ANGLE1)&&(heading_angle_error <= THRESHOLD_ANGLE2))
    {
      robot_control(30,-30);
    }
    else
    {
      robot_control(200,200);
    }

    
    Serial.print("Heading Angle Error : ");
    Serial.print(heading_angle_error);
    Serial.print(" = ");
    Serial.print(target_heading_angle);
    Serial.print(" - ");
    Serial.println(heading);

  }
}

void line_following(int line_type)
{
  switch(line_type)
  {
     case -5:
              robot_control(50+motor_speed_offset,50);  
              break;
     case -4:
              robot_control(-80, 200);//왼  
              break;
     case -3:
              robot_control(-40+motor_speed_offset, 180);  
              break;
     case -2:
              robot_control(-10+motor_speed_offset, 160);  
              break;                            
     case -1:
              robot_control(20+motor_speed_offset, 140);  
              break;
     case 0:
              robot_control(80+motor_speed_offset, 80);//직
              break;
     case 1:
              robot_control(140+motor_speed_offset, 20);
              break;
     case 2:
              robot_control(160+motor_speed_offset, -10);
              break;
     case 3:
              robot_control(180+motor_speed_offset, -40);
              break;
     case 4:
              robot_control(200+motor_speed_offset, -100);//오
              break;
     case 5: //불이 다 들어올 경우 정지
              robot_control(0,0);
              break;
   }
 }


void loop()
{
  // put your main code here, to run repeatedly:
  int i;
  int line_type = 0;
  read_ultrasonic_sensor();
  if(UltrasonicSensorData[0]<10)
  {
    target_heading_angle = 40;
    imu_rotation();
    robot_control(80+motor_speed_offset, 80);//직
    delay(300);
    target_heading_angle = 320;
    imu_rotation();
  }
  line_type = read_digital_line_sensor();  // 함수 실행
  Serial.print("Input data = ");
  for(i=0;i<5;i++)
  {
    Serial.print(linesensor_data[i]);
    Serial.print(" ");
  }
  Serial.print(sum);  // sum 값 출력
  Serial.println(" ");  // 줄 바꾸기
  line_following(line_type);
}
