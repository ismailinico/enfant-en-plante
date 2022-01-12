#include <DallasTemperature.h>
#include <OneWire.h>
#include <Servo.h>
#include "TMRpcm.h"
#include "SPI.h"
#include "SD.h"

/* ANMERKUNG: Der Lichtsensor gibt nur ein Richtwert zurück
 * und ist somit keine wissenschaftliche Auslesung, weshalb
 * die MIN und MAX Werte nach eigenemen Ermessen abgeschätzt
 * werden müssen. Die restlichen Werte können durch testen
 * definiert werden.
*/

// Pin constants
#define SERVO_PIN 3
#define TEMP_SENSOR_PIN 2
#define SPEAKER_PIN 9
#define LIGHT_SENSOR_PIN A1
#define HUMIDITY_SENSOR_PIN A0
#define SD_CARD_PIN 4

// Sensor constants
#define WATER_VALUE 420
#define AIR_VALUE 802
#define SUN_VALUE 1000

// Speaker

// Speaker pitches
#define COLD "cold.wav"
#define HOT "hot.wav"
#define WET "wet.wav"
#define DRY "dry.wav"
#define BRIGHT "bright.wav"
#define DARK "dark.wav"

// Speaker sound lengths
#define MEDIUM 5000
#define WARNING 1000
#define ALARM 100

// Servo

// Chin angles
#define OPEN 0
#define CLOSE 0

// State flags

// Humidity flags
#define WET_ALARM 94
#define WET_WARN 87
#define MAX_HUMIDITY 84
#define MIN_HUMIDITY 72
#define DRY_WARN 45
#define DRY_ALARM 15

// Brightness flags
#define BRIGHT_ALARM 90
#define BRIGHT_WARN 85
#define MAX_BRIGHTNESS 80
#define MIN_BRIGHTNESS 30
#define DARK_WARN 15
#define DARK_ALARM 1

// Temperature flags
#define HOT_ALARM 31
#define HOT_WARN 28
#define MAX_TEMP 26
#define MIN_TEMP 15
#define COLD_WARN 9
#define COLD_ALARM 5

Servo servo1;
TMRpcm tmrpcm;
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);

void setup()
{
  // Set mode of analog pin
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  // Attach servo to digital pin 3 and set it to 0
  servo1.attach(SERVO_PIN);
  // Begin accepting data from serial bus
  Serial.begin(9600);
  // Begin reading data from temperature sensor
  sensors.begin();
  tmrpcm.speakerPin = SPEAKER_PIN;
  if (!SD.begin(SD_CARD_PIN)) {
    Serial.println("SD fail");
  }
}

void loop()
{
 Serial.print("\nHumidity: ");
 Serial.print(getHumidity());
 Serial.print("%\nBrigtness: ");
 Serial.print(getBrightness());
 Serial.print("%\nTemperature: ");
 Serial.print(getTemp());
 Serial.print("°C\n");
 checkBounds(getHumidity(), WET, MAX_HUMIDITY, WET_WARN, WET_ALARM, DRY, MIN_HUMIDITY, DRY_WARN, DRY_ALARM);
 checkBounds(getBrightness(), BRIGHT, MAX_BRIGHTNESS, BRIGHT_WARN, BRIGHT_WARN, DARK, MIN_BRIGHTNESS, DARK_WARN, DARK_ALARM);
 checkBounds(getTemp(), HOT, MAX_TEMP, HOT_WARN, HOT_ALARM, COLD, MIN_TEMP, COLD_WARN, COLD_ALARM);
}

void checkBounds(int input, const char* maxSound, int maxValue, int warnMaxValue, int alertMaxValue, const char* minSound, int minValue, int warnMinValue, int alertMinValue) {
  int delayTime = 0;
  char* sound = NULL;
  
  if(input > maxValue) {
    sound = maxSound;
    if (input >= alertMaxValue)
    {
      delayTime = ALARM;
    }
    else if (alertMaxValue > input && input > warnMaxValue)
    {
      delayTime = WARNING;
    }
    else
    {
      delayTime = MEDIUM;
    }
 }
 else if (input < minValue)
 {
  sound = minSound;
  if (input <= alertMinValue)
    {
      delayTime = ALARM;
    }
    else if (alertMinValue < input && input < warnMinValue)
    {
      delayTime = WARNING;
    }
    else
    {
      delayTime = MEDIUM;
    }
 }
 playSound(sound, delayTime);
}

int getTemp() {
 sensors.requestTemperatures();
 return sensors.getTempCByIndex(0);
}

int getBrightness() {
  int lightPercentage = map(analogRead(LIGHT_SENSOR_PIN), 0, SUN_VALUE, 0, 100);
  if (lightPercentage >= 100)
  {
    return 1;
  }
  else if (lightPercentage <= 0)
  {
    return 0;
  }
  else
  {
    return lightPercentage;
  }
}

int getHumidity() {
  int humidityInPercent = map(analogRead(HUMIDITY_SENSOR_PIN), AIR_VALUE, WATER_VALUE, 0, 100);
  if (humidityInPercent >= 100)
  {
    return 1;
  }
  else if (humidityInPercent <= 0)
  {
    return 0;
  }
  else
  {
    return humidityInPercent;
  }
}

/**
 * Plays a pitch for 100ms and activates servo.
 */
void playSound(const char* sound, int delayTime) {
  tmrpcm.setVolume(6);
  tmrpcm.play(sound);
}
