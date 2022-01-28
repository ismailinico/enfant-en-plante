#include "TMRpcm.h"
TMRpcm tmrpcm;
void setup()
{ tmrpcm.setVolume(5);
  tmrpcm.speakerPin = 8;
  if (!SD.begin(10)) {
    Serial.println("SD fail");
  }
  
}

void loop()
{
  char* sound = strdup("hmaxok.wav");
  tmrpcm.play(sound);
  delay(5000);
}
