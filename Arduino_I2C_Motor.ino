const int dirPin = 8;
const int stepPin = 9;
const int enablePin = 10;

const int RPM = 1;
const int StepsPerRev = 1600;
const long StepsPerMin = (long)StepsPerRev*(long)RPM;
const long DelayMicro = 60000000/StepsPerMin;



void setup() {
  Serial.begin(9600);

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  
  digitalWrite(enablePin, LOW);
  digitalWrite(dirPin, LOW);
  digitalWrite(stepPin, LOW);

}

void loop() {

  digitalWrite(stepPin, HIGH);
  delay(DelayMicro);
  digitalWrite(stepPin, LOW);
  delay(DelayMicro);

}
