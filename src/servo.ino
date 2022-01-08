#include <Servo.h>
Servo servo1;

void setup()
{
  servo1.attach(3);
  servo1.write(180);
}

void loop()
{
}
