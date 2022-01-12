#include <Servo.h>
Servo servo1;

void setup()
{
  servo1.attach(3);
}

void loop()
{
  servo1.write(120);
  tone(4, 100);
  delay(1000);
  servo1.write(30);
  noTone(4);
  delay(1000);
}
