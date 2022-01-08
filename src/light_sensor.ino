const int BRIGHT_VAL = 1000;
int lightValue = 0;
int lightPercentage = 0;
int sensorPin = A1;

void setup()
{
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

// 90% > optimal > 70%

void loop()
{
  lightValue = analogRead(sensorPin);
  lightPercentage = map(lightValue, 0, BRIGHT_VAL, 0, 100);
  if (lightPercentage >= 100)
  {
    Serial.println("100 %");
  }
  else if (lightPercentage <= 0)
  {
    Serial.println("0 %");
  }
  else if (lightPercentage > 0 && lightPercentage < 100)
  {
    Serial.print(lightPercentage);
    Serial.println("%");
  }
  delay(1000);
}
