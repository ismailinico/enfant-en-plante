#include <DallasTemperature.h>
#include <OneWire.h>
#define B20_Signal_PIN 2

OneWire oneWire(B20_Signal_PIN);
DallasTemperature sensors(&oneWire);

void setup()
{
  Serial.begin(9600);
  sensors.begin();
}

void loop()
{
  sensors.requestTemperatures();
  Serial.print("Temperatur: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.println(" Grad Celsius");
  delay(1000);
}
