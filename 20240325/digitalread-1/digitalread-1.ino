#define ledpin 13
#define inpin 7
int val=0;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledpin,OUTPUT);
  pinMode(inpin,INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  val=digitalRead(inpin);

  Serial.print("input data=");
  Serial.println(val);
  digitalWrite(ledpin,val);
}
