#include <NewPing.h>

#define TRIG_PIN1 11
#define ECHO_PIN1 12
#define TRIG_PIN2 9
#define ECHO_PIN2 8
#define TRIG_PIN3 7
#define ECHO_PIN3 6

#define MAX_DISTANCE 5 // Maximum distance for car detection (in centimeters)

NewPing sonar1(TRIG_PIN1, ECHO_PIN1, MAX_DISTANCE);
NewPing sonar2(TRIG_PIN2, ECHO_PIN2, MAX_DISTANCE);
NewPing sonar3(TRIG_PIN3, ECHO_PIN3, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned int distance1 = sonar1.ping_cm();
  unsigned int distance2 = sonar2.ping_cm();
  unsigned int distance3 = sonar3.ping_cm();

 
  // Spot 1
  if (distance1 > 0 && distance1 < MAX_DISTANCE) {
    Serial.print("1"); //Spot 1 is occupied
  } else {
    Serial.print("0"); //Spot 1 is available
  }

  // Spot 2
  if (distance2 > 0 && distance2 < MAX_DISTANCE) {
    Serial.print("1"); //Spot 2 is occupied
  } else {
    Serial.print("0"); //spot 2 is available 
  }

  // Spot 3
  if (distance3 > 0 && distance 3 MAX_DISTANCE) {
    Serial.print("1"); //spot 3 is occupied
  } else {
    Serial.print("0"); //spot 3 is available
  }

  delay(5000); // Delay for 5 seconds to indicate Spot 2 becoming occupied
}
