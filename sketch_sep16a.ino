#include <Bonezegei_LCD1602_I2C.h>

Bonezegei_LCD1602_I2C lcd(0x27);


const int dirPin = 8;
const int stepPin = 9;
const int enablePin = 10;

int thermistor = 0;
int heater = 7;
int TEMPERATURE;

int RPM = 5;
int StepsPerRev = 800;
long StepsPerMin = StepsPerRev*RPM;
long DelayMilli = (60*1000)/StepsPerMin;


void setup() {
  Serial.begin(9600);

  lcd.begin();

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(enablePin, LOW);
  digitalWrite(dirPin, LOW);
  digitalWrite(stepPin, LOW);
}


void loop() {

  for (int i = 0; i < StepsPerRev; i++) {
    digitalWrite(stepPin, HIGH);
    delay(DelayMilli);
    digitalWrite(stepPin, LOW);
    delay(DelayMilli);
  }

} 


void loop() {

  TEMPERATURE = analogRead(thermistor);
  Serial.println(TEMPERATURE);

  if(TEMPERATURE<796){
    digitalWrite(heater, HIGH);
    Serial.println(Unterer Schwellenwert erreicht!);
  }

  if(TEMPERATURE>806){
    digitalWrite(heater, LOW);
    Serial.println(Oberer Schwellenwert erreicht!);
  }
  delay(250);

}


void loop() {
  
  lcd.setBacklight(1);
  lcd.setPosition(0, 0);
  lcd.print("Temperatur:" TEMPERATURE );
  lcd.setPosition(0, 1);
  lcd.print("RPM:" RPM );

}


