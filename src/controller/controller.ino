#include <DallasTemperature.h>
#include <OneWire.h>
#include"ServoTimer2.h"
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
#define LIGHT_SENSOR_PIN A0
#define HUMIDITY_SENSOR_PIN A1
#define SD_CARD_PIN 10

// Sensor constants
#define WATER_VALUE 420
#define AIR_VALUE 802
#define SUN_VALUE 1000

// Speaker

// Sounds
// Humidity
#define H_MAX_ALARM "hmaxal.wav"
#define H_MAX_WARN "hmaxwa.wav"
#define H_MAX_OK "hmaxok.wav"
#define H_MAX_GOOD "hgood.wav"
#define H_MIN_GOOD "hgood.wav"
#define H_MIN_OK "hminok.wav"
#define H_MIN_WARN "hminwa.wav"
#define H_MIN_ALARM "hminal.wav"
// Light 
#define L_MAX_ALARM "lmaxal.wav"
#define L_MAX_WARN "lmaxwa.wav"
#define L_MAX_OK "lmaxok.wav"
#define L_MAX_GOOD "lgood.wav"
#define L_MIN_GOOD "lgood.wav"
#define L_MIN_OK "lminok.wav"
#define L_MIN_WARN "lminwa.wav"
#define L_MIN_ALARM "lminal.wav"
// Temperature
#define T_MAX_ALARM "tmaxal.wav"
#define T_MAX_WARN "tmaxwa.wav"
#define T_MAX_OK "tmaxok.wav"
#define T_MAX_GOOD "tgood.wav"
#define T_MIN_GOOD "tgood.wav"
#define T_MIN_OK "tminok.wav"
#define T_MIN_WARN "tminwa.wav"
#define T_MIN_ALARM "tminal.wav"

// Servo

// Chin angles
#define OPEN 2700
#define CLOSE 1900

// State flags

// Humidity flags
#define WET_ALARM 99
#define WET_WARN 97
#define MAX_HUMIDITY 95
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

ServoTimer2 myservo;
TMRpcm tmrpcm;
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);
int prev_humidity = -1, prev_light = -1, prev_temp = -1;

void setup()
{
  // Set mode of analog pin
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  // Begin accepting data from serial bus
  Serial.begin(9600);
  // Begin reading data from temperature sensor
  sensors.begin();
  tmrpcm.setVolume(6);
  tmrpcm.speakerPin = SPEAKER_PIN;
  if (!SD.begin(SD_CARD_PIN)) {
    Serial.println("SD fail");
  }
  // Attach servo to digital pin 3 and set it to 0
  myservo.attach(SERVO_PIN);
  myservo.write(CLOSE);
}

void loop()
{
int humidity = getHumidity(), light = getBrightness(), temp = getTemp();

 Serial.print("\n\nPrevious humidity[%]: ");
 Serial.print(prev_humidity);
 Serial.print("\nPrevious brigtness[%]: ");
 Serial.print(prev_light);
 Serial.print("\nPrevious temperature[°C]: ");
 Serial.print(prev_temp);
 
 Serial.print("\n\nHumidity[%]: ");
prev_humidity = checkBounds(humidity, prev_humidity,
            MAX_HUMIDITY, WET_WARN, WET_ALARM,
            H_MAX_GOOD, H_MAX_OK,
            H_MAX_WARN, H_MAX_ALARM,
            MIN_HUMIDITY, DRY_WARN, DRY_ALARM,
            H_MIN_GOOD, H_MIN_OK,
            H_MIN_WARN, H_MIN_ALARM);
 delay(2000);
  Serial.print("\nBrigtness[%]: ");
 prev_light = checkBounds(light, prev_light,
            MAX_BRIGHTNESS, BRIGHT_WARN, BRIGHT_ALARM,
            L_MAX_GOOD, L_MAX_OK,
            L_MAX_WARN, L_MAX_ALARM,
            MIN_BRIGHTNESS, DARK_WARN, DARK_ALARM,
            L_MIN_GOOD, L_MIN_OK,
            L_MIN_WARN, L_MIN_ALARM);
 delay(2000);
 Serial.print("\nTemperature[°C]: ");
 prev_temp = checkBounds(temp, prev_temp,
            MAX_TEMP, HOT_WARN, HOT_ALARM,
            T_MAX_GOOD, T_MAX_OK,
            T_MAX_WARN, T_MAX_ALARM,
            MIN_TEMP, COLD_WARN, COLD_ALARM,
            T_MIN_GOOD, T_MIN_OK,
            T_MIN_WARN, T_MIN_ALARM);
 delay(2000);
}

int checkBounds(int input, int prevInput,
        int maxValue, int warnMaxValue, int alarmMaxValue, 
        char const *goodMaxSound, char const *okMaxSound,
        char const *warnMaxSound, char const *alarmMaxSound,
        int minValue, int warnMinValue, int alarmMinValue,
        char const *goodMinSound, char const *okMinSound,
        char const *warnMinSound, char const *alarmMinSound) {
  char* sound = NULL;
  Serial.print(input);
  if (input < maxValue && input > minValue) {
    if(prevInput == -1) {
      return input;
    }
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
  delay(10);
  if (sound != NULL) {
    Serial.print("\nNow playing ");
    Serial.print(sound);
    tmrpcm.play(sound);
    delay(10);
    myservo.write(OPEN);
    delay(5000);
    myservo.write(CLOSE);
  }
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
