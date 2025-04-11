#include <Servo.h>
#define TRIG_PIN 4
#define ECHO_PIN 5
#define SERVO_PIN 10
#define DIST 8

Servo windowServo;
int pos = 0;

void toggleServo(int *pos) {
  if((*pos) == 1)
  {
    windowServo.write(180);
    (*pos) = 0;
  } else {
    windowServo.write(0);
    (*pos) = 1;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  windowServo.attach(SERVO_PIN);
  windowServo.write(0);
}

void loop() {
  long duration;
  int distance;

  // Trigger the sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo time
  duration = pulseIn(ECHO_PIN, HIGH);

  // Convert to distance in cm
  distance = duration * 0.034 / 2;

  // Open or close the window 
  if (distance < DIST) {
      toggleServo(&pos);
      delay(2000);
  }

  // Print to serial
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500); // Update every half second
}
