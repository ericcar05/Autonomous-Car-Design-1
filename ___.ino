#define SONIC_TRIG 2
#define SONIC_ECHO 3

void setup() {
  Serial.begin(9600);
  pinMode(SONIC_TRIG, OUTPUT);
  pinMode(SONIC_ECHO, INPUT);
}

void loop(){
  digitalWrite(SONIC_TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(SONIC_TRIG,LOW);

  unsigned long duration = pulseIn(SONIC_ECHO,HIGH);
  unsigned long distance = duration/58;
  Serial.print(duration);
  Serial.print(",");
  Serial.print(distance);
  Serial.println("cm");
  delay(25);
}
