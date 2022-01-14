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

// Sounds
// Humidity
#define H_MAX_ALARM "wet.wav"
#define H_MAX_WARN "wet.wav"
#define H_MAX_OK "wet.wav"
#define H_MAX_GOOD "humidity.wav"
#define H_MIN_GOOD "humidity.wav"
#define H_MIN_OK "dry.wav"
#define H_MIN_WARN "dry.wav"
#define H_MIN_ALARM "dry.wav"
// Light 
#define L_MAX_ALARM "bright.wav"
#define L_MAX_WARN "bright.wav"
#define L_MAX_OK "bright.wav"
#define L_MAX_GOOD "light.wav"
#define L_MIN_GOOD "light.wav"
#define L_MIN_OK "dark.wav"
#define L_MIN_WARN "dark.wav"
#define L_MIN_ALARM "dark.wav"
// Temperature
#define T_MAX_ALARM "hot.wav"
#define T_MAX_WARN "hot.wav"
#define T_MAX_OK "hot.wav"
#define T_MAX_GOOD "temp.wav"
#define T_MIN_GOOD "temp.wav"
#define T_MIN_OK "cold.wav"
#define T_MIN_WARN "cold.wav"
#define T_MIN_ALARM "cold.wav"

// Servo

// Chin angles
#define OPEN 0
#define CLOSE 0

// State flags

// Humidity flags
#define WET_ALARM 98
#define WET_WARN 95
#define MAX_HUMIDITY 90
#define MIN_HUMIDITY 70
#define DRY_WARN 45
#define DRY_ALARM 15

// Brightness flags
#define BRIGHT_ALARM 98
#define BRIGHT_WARN 95
#define MAX_BRIGHTNESS 92
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

int prev_humidity = MIN_HUMIDITY, prev_light = MIN_BRIGHTNESS, prev_temp = MIN_TEMP;

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
  tmrpcm.setVolume(6);
  tmrpcm.speakerPin = SPEAKER_PIN;
  if (!SD.begin(SD_CARD_PIN)) {
    Serial.println("SD fail");
  }
}

void loop()
{
int humidity = getHumidity(), light = getBrightness(), temp = getTemp();
 Serial.print("\nPrevious humidity: ");
 Serial.print(prev_humidity);
 Serial.print("%\nPrevious brigtness: ");
 Serial.print(prev_light);
 Serial.print("%\nPrevious temperature: ");
 Serial.print(prev_temp);
 Serial.print("°C\n");
 Serial.print("\nHumidity: ");
 Serial.print(humidity);
 Serial.print("%\nBrigtness: ");
 Serial.print(light);
 Serial.print("%\nTemperature: ");
 Serial.print(temp);
 Serial.print("°C\n");
prev_humidity = checkBounds(humidity, prev_humidity,
            MIN_HUMIDITY, WET_WARN, WET_ALARM,
            H_MAX_GOOD, H_MAX_OK,
            H_MAX_WARN, H_MAX_ALARM,
            MAX_HUMIDITY, DRY_WARN, DRY_ALARM,
            H_MIN_GOOD, H_MIN_OK,
            H_MIN_WARN, H_MIN_ALARM);
 delay(4000);
 prev_light = checkBounds(light, prev_light,
            MAX_BRIGHTNESS, BRIGHT_WARN, BRIGHT_ALARM,
            L_MAX_GOOD, L_MAX_OK,
            L_MAX_WARN, L_MAX_ALARM,
            MIN_BRIGHTNESS, DARK_WARN, DARK_ALARM,
            L_MIN_GOOD, L_MIN_OK,
            L_MIN_WARN, L_MIN_ALARM);
 delay(4000);
 prev_temp = checkBounds(temp, prev_temp,
            MAX_TEMP, HOT_WARN, HOT_ALARM,
            T_MAX_GOOD, T_MAX_OK,
            T_MAX_WARN, T_MAX_ALARM,
            MIN_TEMP, COLD_WARN, COLD_ALARM,
            T_MIN_GOOD, T_MIN_OK,
            T_MIN_WARN, T_MIN_ALARM);
  delay(4000);
}

int checkBounds(int input, int prevInput,
        int maxValue, int warnMaxValue, int alarmMaxValue, 
        char const *goodMaxSound, char const *okMaxSound,
        char const *warnMaxSound, char const *alarmMaxSound,
        int minValue, int warnMinValue, int alarmMinValue,
        char const *goodMinSound, char const *okMinSound,
        char const *warnMinSound, char const *alarmMinSound) {
          
  char* sound = NULL;
  if (input < maxValue && input > minValue) {
    if(prevInput > maxValue) {
      sound = strdup(goodMaxSound);
    }
    else if(prevInput < minValue){
      sound = strdup(goodMinSound);
    }
   }
  else if(input > maxValue) {
    if (input >= alarmMaxValue)
    {
      sound = strdup(warnMaxSound);
    }
    else if (alarmMaxValue > input && input > warnMaxValue)
    {
      sound = strdup(alarmMaxSound);
    }
    else
    {
      sound = strdup(okMaxSound);
    }
   }
   else if (input < minValue)
   {
    if (input <= alarmMinValue)
      {
        sound = strdup(warnMinSound);
      }
      else if (alarmMinValue < input && input < warnMinValue)
      {
        sound = strdup(alarmMinSound);
      }
      else
      {
        sound = strdup(okMinSound);
      }
   }
 tmrpcm.play(sound);
 return input;
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
