#include <Servo.h>

Servo myservo;
int pos = 0;

void setup() {
  myservo.attach(3);
}
void loop() {

  for (pos = 120; pos <= 180; pos += 1) { 
    myservo.write(pos);
    delay(5);
  }

  for (pos = 180; pos >= 120; pos -= 1) {
    myservo.write(pos);
    delay(5); 
  }

}
