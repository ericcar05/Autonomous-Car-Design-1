#include <NewPing.h>
#include <Wire.h>
#include <LSM303.h>

#define THRESHOLD_ANGLE1 15
#define THRESHOLD_ANGLE2 7

#define motor_speed_offset 30
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

#define NO_MISSION 14  // 미션의 갯수를 정의 , 직진, 회전, 직진, 회전 등등

bool mission_flag[NO_MISSION]= {0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

int flag_runner = -1; // 0번 라인 following / 1번 Maze / 2번 장애물 회피

LSM303 compass;

float heading_angle = 0.0;
float in_heading_angle = 17.0;
float target_heading_angle = 90;
float heading_angle_error = 0.0;

NewPing sonar_front(FTRIGGER_PIN, FECHO_PIN, MAX_DISTANCE);
NewPing sonar_left(LTRIGGER_PIN, LECHO_PIN, MAX_DISTANCE);
NewPing sonar_right(RTRIGGER_PIN, RECHO_PIN, MAX_DISTANCE);

const int numReadings = 5;
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

void Sonar_Data_Display(int flag)
{
  char Sonar_data_display[40];
  if(flag==0) return;
  else
  {
    sprintf(Sonar_data_display,"F:");
    Serial.print(Sonar_data_display);
    Serial.print(UltrasonicSensorData[0]);
    Serial.print("B:");
    Serial.print(UltrasonicSensorData[1]);
    Serial.print("R:");
    Serial.print(UltrasonicSensorData[2]);
    Serial.print("L:");
    Serial.println(UltrasonicSensorData[3]);
  }
}

void setup()
{
    Serial.begin(9600);
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

void wall_following_l(int distance,int base_speed)
{
  int i;
  float kp=3.1;
  float kd=10;
  int l_speed=0;
  int r_speed=0;
  float error=0;
  float d_error=0;
  float error_old=0;
  float speed_control;
  int speed_control_max=15;
  read_ultrasonic_sensor();
  Sonar_Data_Display(0);
  error=UltrasonicSensorData[1]-distance;
  d_error=error-error_old;
  speed_control=kp*error+kd*d_error;
  if(speed_control>=speed_control_max) speed_control=speed_control_max;
  if(speed_control<= -speed_control_max) speed_control=-speed_control_max;
  l_speed=base_speed+speed_control;
  r_speed=base_speed-speed_control;
  error_old=error;

  Serial.print(error);
  Serial.print("  ");
  Serial.print(l_speed);
  Serial.print("  ");
  Serial.println(r_speed);
  robot_control(l_speed+motor_speed_offset,r_speed);
}


void wall_following_r(int distance,int base_speed)
{
  int i;
  float kp=3.1;
  float kd=10;
  int l_speed=0;
  int r_speed=0;
  float error=0;
  float d_error=0;
  float error_old=0;
  float speed_control;
  int speed_control_max=15;
  read_ultrasonic_sensor();
  Sonar_Data_Display(0);
  error=UltrasonicSensorData[2]-distance;
  d_error=error-error_old;
  speed_control=kp*error+kd*d_error;
  if(speed_control>=speed_control_max) speed_control=speed_control_max;
  if(speed_control<= -speed_control_max) speed_control=-speed_control_max;
  l_speed=base_speed-speed_control;
  r_speed=base_speed+speed_control;
  error_old=error;

  Serial.print(error);
  Serial.print("  ");
  Serial.print(l_speed);
  Serial.print("  ");
  Serial.println(r_speed);
  robot_control(l_speed+motor_speed_offset,r_speed);
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

void loop()
{
  if(mission_flag[0]==0)
  {
    wall_following_r(6,100);
    read_ultrasonic_sensor();
    if(laverage>35)
    {
      mission_flag[0] = 1;
      mission_flag[1] = 0;
    }  
  }
  if(mission_flag[1]==0)
  {
    robot_control(-255, 255);
    delay(260);            // 회전하는 시간// 오른쪽 회전
    mission_flag[1] = 1;
    mission_flag[2] = 0;
  }
  if(mission_flag[2]==0)
  {
    wall_following_r(6,100);
    read_ultrasonic_sensor();
    if((laverage>15)&&(raverage>15))
    {
      mission_flag[2] = 1;
      mission_flag[3] = 0;
    }     
  }
  if(mission_flag[3]==0)
  {
    robot_control(100+motor_speed_offset,100);
    read_ultrasonic_sensor();    
    if((laverage<15)&&(raverage<15))
    {
      mission_flag[3] = 1;
      mission_flag[4] = 0;
    }     
  }
  if(mission_flag[4]==0)
  {
    wall_following_r(6,100);
    read_ultrasonic_sensor();
    if((raverage>35)&&(faverage<16))
    {
      mission_flag[4] = 1;
      mission_flag[5] = 0;
    }   
  }
  if(mission_flag[5]==0)
  {
    robot_control(255, -255);
    delay(320);            // 회전하는 시간// 오른쪽 회전
    mission_flag[5] = 1;
    mission_flag[6] = 0; 
  }
  if(mission_flag[6]==0)
  {
    wall_following_r(6,100);
    read_ultrasonic_sensor();
    if((faverage<=16)&&((laverage<=15)||(raverage<=15)))
    {
      mission_flag[6] = 1;
      mission_flag[7] = 0;
    }   
  }
  if(mission_flag[7]==0)
  {
    robot_control(255, -255);
    delay(600);// 180도 회전(오른/왼쪽)
    mission_flag[7] = 1;
    mission_flag[8] = 0;  
  }
  if(mission_flag[8]==0)
  {
    wall_following_r(6,70);
    read_ultrasonic_sensor();
    if(laverage>35)
    {
      mission_flag[8] = 1;
      mission_flag[9] = 0;
    }  
  }
  if(mission_flag[9]==0)
  {
    robot_control(-255, 255);
    delay(255);// 180도 회전(오른/왼쪽)
    mission_flag[9] = 1;
    mission_flag[10] = 0;  
  }
  if(mission_flag[10]==0)
  {
    wall_following_l(6,100);
    read_ultrasonic_sensor();
    if((laverage>15)&&(raverage>15))
    {
      mission_flag[10] = 1;
      mission_flag[11] = 0;
    }  
  }
  if(mission_flag[11]==0)
  {
    robot_control(100+motor_speed_offset,100);
    read_ultrasonic_sensor();    
    if((laverage<15)&&(raverage<15))
    {
      mission_flag[11] = 1;
      mission_flag[12] = 0;
    }     
  }
  if(mission_flag[12]==0)
  {
    wall_following_r(6,70);
    read_ultrasonic_sensor();
    if((faverage<=16)&&((laverage>=35)||(raverage>=35)))
    {
      mission_flag[12] = 1;
      mission_flag[13] = 0;
    }  
  }
  if(mission_flag[13]==0)
  {
    robot_control(-255, 255);
    delay(255);// 180도 회전(오른/왼쪽)
    mission_flag[13] = 1;
    mission_flag[14] = 0;  
  }
  if(mission_flag[14]==0)
  {
    wall_following_r(6,200);
    read_ultrasonic_sensor(); 
  }
  else
  {
    robot_control(255,255);
  }
  
}
