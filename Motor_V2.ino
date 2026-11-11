const int dirPin = 8;
const int stepPin = 9;
const int enablePin = 10;

int thermistor = 0;
int heater = 7;
int TEMPERATURE;

const int RPM = 1;
const int StepsPerRev = 1600;
const long StepsPerMin = (long)StepsPerRev*(long)RPM;
const long DelayMicro = 60000000/StepsPerMin;

unsigned long previousMillis = 0;
const long interval = 500;


void setup() {
  Serial.begin(115200);

  //lcd.begin();

//  pinMode(button, INPUT);
  pinMode(heater, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(heater, HIGH);
  digitalWrite(enablePin, LOW);
  digitalWrite(dirPin, LOW);
  digitalWrite(stepPin, LOW);

}


void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis%500 == 0){

    TEMPERATURE = analogRead(thermistor);
    Serial.println(TEMPERATURE);

    if(TEMPERATURE<796){
      digitalWrite(heater, HIGH);
      Serial.println("Unterer Schwellenwert erreicht!");
    }

    if(TEMPERATURE>806){
      digitalWrite(heater, LOW);
      Serial.println("Oberer Schwellenwert erreicht!");
    }

  }

  digitalWrite(stepPin, HIGH);
  delay(DelayMicro);
  digitalWrite(stepPin, LOW);
  delay(DelayMicro);

}