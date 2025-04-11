#include <Servo.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;

// Define Pins
#define DHTPIN A0
#define SERVO_PIN 9


Servo windowServo;
int pos = 0;

// Temperature threshold
const float TEMP_THRESHOLD = 24.0;

void setup() {
  Serial.begin(9600);
  windowServo.attach(SERVO_PIN);
  windowServo.write(0);  // Start with window closed
}


void toggleServo(int *pos) {
  if ((*pos) == 1) {
    windowServo.write(180);
    (*pos) = 0;
  } else {
    windowServo.write(90);
    (*pos) = 1;
  }
}

void loop() {

  // Read data from sensor
  DHT.read(DHTPIN);
  int temperature = DHT.temperature;

  Serial.print("temp:");
  Serial.print(temperature);
  Serial.print(" humi:");
  Serial.println(DHT.humidity);

  // Open and close the window depending on the temp
  if (temperature < TEMP_THRESHOLD) {
    toggleServo(&pos);
    Serial.print(pos);
  } else {
        toggleServo(&pos);
  }

  delay(1000); // Wait 1 second before next read
}
