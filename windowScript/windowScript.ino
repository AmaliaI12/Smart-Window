#include <Servo.h>
#include <DFRobot_DHT11.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address
DFRobot_DHT11 DHT;

// Define Pins
#define DHTPIN A0
#define TRIG_PIN 4
#define ECHO_PIN 5
#define DIST 8
#define TEMP__SERVO 9
#define DIST_SERVO 10


Servo tempServo;
Servo distServo;
static bool openPartially = false;
static bool openFully = false;


// Temperature threshold
const float TEMP_THRESHOLD = 24.0;

void toggleServo() {
  if(openFully == 1)
  {
    distServo.write(0);
    lcd.setCursor(2, 1);
    lcd.print("Closing...      ");
    delay(1000);
    lcd.setCursor(2, 1);
    lcd.print("CLOSED         ");
    openFully = 0;
  } else {
    tempServo.write(0);
    delay(500);
    distServo.write(180);
    lcd.setCursor(2, 1);
    lcd.print("Opening...       ");
    delay(1000);
    lcd.setCursor(2, 1);
    lcd.print("OPEN           ");
    openFully = 1;
    openPartially = 0;
  }
}

void setup() {
  Serial.begin(9600);

  tempServo.attach(TEMP__SERVO);
  tempServo.write(0);  // Start with window closed

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  distServo.attach(DIST_SERVO);
  distServo.write(0);


  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.setCursor(2, 1);
  lcd.print("CLOSED         ");
}

void loop() {

  // Read data from sensor
  DHT.read(DHTPIN);
  int temp = DHT.temperature;

  Serial.print("temp:");
  Serial.print(temp);

  lcd.setCursor(2, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C   ");

  // Open and close the window depending on the temp
  if (temp > TEMP_THRESHOLD) {
      if (!openPartially && !openFully) {
        tempServo.write(180);
        lcd.setCursor(2, 1);
        lcd.print("Opening...   ");
        delay(1000);
        lcd.setCursor(2, 1);
        lcd.print("PARTIALLY OPEN  ");
        openPartially = 1;
      }
      
  } else {
      if (openPartially) {
        tempServo.write(0);
        lcd.setCursor(2, 1);
        lcd.print("Closing...   ");
        delay(1000);
        lcd.setCursor(2, 1);
        lcd.print("CLOSED           ");
        openPartially = 0;
      }
  }

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
      toggleServo();
      delay(2000);
  }

  // Print to serial
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500); // Update every half second
}
