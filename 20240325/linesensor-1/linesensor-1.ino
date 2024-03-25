#define ledpin 13
int linesensor_data[5]={0,0,0,0,0};
int linesensor_pin[5]={2,3,4,5,6};
void setup() {
  int i;
  // put your setup code here, to run once:
  pinMode(ledpin,OUTPUT);
  for(i=0;i<5;i++)
  {
    pinMode(linesensor_pin[i],INPUT);
  }
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int i;
   for(i=0;i<5;i++)
  {
    linesensor_data[i]=digitalRead(linesensor_pin[i]);
  }

  Serial.print("input data=");
  for(i=0;i<5;i++)
  {
    Serial.print(linesensor_data[i]);
    Serial.print(" ");
  }
  Serial.println(" ");
}
