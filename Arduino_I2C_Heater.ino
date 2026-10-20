int thermistor = 0;
int heater = 7;
int TEMPERATURE;

void setup() {

  Serial.begin(9600);

  pinMode(heater, OUTPUT);
  digitalWrite(heater, HIGH);
} 

void loop() {

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
delay(250);

}
