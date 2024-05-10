#include <NewPing.h>

#define FTRIGGER_PIN 14
#define FECHO_PIN 15
#define LTRIGGER_PIN 17
#define LECHO_PIN 16
#define RTRIGGER_PIN 19
#define RECHO_PIN 18
#define MAX_DISTANCE 100

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

void setup()
{
    Serial.begin(9600);
    for (int thisReading = 0; thisReading < numReadings; thisReading++)
    { 
      freadings[thisReading] = 0;
      lreadings[thisReading] = 0;
      rreadings[thisReading] = 0;
    }
}
void moving_average_filter()
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
  readIndex++;                  // 신호값을 읽은 인덱스를 1 증가 시킨다.​
  if (readIndex >= numReadings)
  {     
    readIndex = 0;                              // 0으로 만들어 처음부터 다시 시작한다
  }
  faverage = ftotal / numReadings;    // 평균값을 구한다
  laverage = ltotal / numReadings;
  raverage = rtotal / numReadings;
}

void loop()
{
    moving_average_filter();
    distance = faverage;
    ldistance = laverage;
    rdistance = raverage;
    if(distance<=5)
      distance=MAX_DISTANCE;
    if(ldistance<=5)
      ldistance=MAX_DISTANCE;
    if(rdistance<=5)
      rdistance=MAX_DISTANCE;
    Serial.print("F: ");
    Serial.print(distance);
    Serial.print(" L: "); 
    Serial.print(ldistance);
    Serial.print(" R: "); 
    Serial.println(rdistance);
}
