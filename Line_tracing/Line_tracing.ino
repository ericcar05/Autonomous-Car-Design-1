#define LINE_DETECT_WHITE  1

#define motor_speed_offset 40

#define IN1 29
#define IN2 28
#define ENR 10

#define IN3 30
#define IN4 31
#define ENL 11

int linesensor_data[5] = {0,0,0,0,0};  // 읽은 값을 저장할 변수
int linesensor_pin[5] = {2,3,4,5,6};   // int형 배열
int sum;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  int i;
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
