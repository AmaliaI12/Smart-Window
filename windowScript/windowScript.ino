#include <Servo.h>
#include <DFRobot_DHT11.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address
DFRobot_DHT11 DHT;

// Define Pins

// Temperature sensor
#define DHTPIN A0

// Distance sensor
#define TRIG_PIN 4
#define ECHO_PIN 5

// Motor driver
#define IN2 7
#define IN1 8
#define ENA 9

// Servomotors
#define TEMP_SERVO 10
#define DIST_SERVO 11

//Thresholds
#define DIST_THRESHOLD 8
#define TEMP_THRESHOLD 23
#define VENT_THRESHOLD 25

Servo distServo;
Servo tempServo;

// Initial state
static bool isHalfOpen = false;
static bool isOpen = false;
static bool isVentilating = false;

// Toggle ventilation
void ventilateRoom() {
  if (isVentilating == 0) {
    // Action DC motor
    analogWrite(ENA, 255);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    // Update ventilation state variable
    isVentilating = 1;

  } else {
    // Stop motor after movement
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    
    //Update ventilation state variable
    isVentilating = 0;
  }
}


void slideWindow() {
  if(isHalfOpen)  // The window is half open
  {
    // Slide down the window
    lcd.setCursor(2, 1);
    lcd.print("SLIDING DOWN...       "); //Display action message
    delay(1000);

    tempServo.write(0);
    delay(1250);
  
    // Stop the servo
    tempServo.write(90);

    lcd.setCursor(2, 1);
    lcd.print("CLOSED            "); //Display action message

    // Update window state variable
    isHalfOpen = 0;

  } else {
    lcd.setCursor(2, 1);
    lcd.print("SLIDING UP...            "); // Display action message
  
    delay(1000);

    // Slide up the window
    tempServo.write(180);
    delay(1350);

    // Stop the servo
    tempServo.write(90);

    lcd.setCursor(2, 1);
    lcd.print("HALF OPEN            ");

    isHalfOpen = 1;
    }
}


void openWindow() {
  if(isOpen)
  {
    lcd.setCursor(2, 1);
    lcd.print("CLOSING...          ");
    delay(1000);

    // Close the window
    for (int pos = 35 ; pos >= 0; pos--) {
        distServo.write(pos);
        delay(30);
    }

    lcd.setCursor(2, 1);
    lcd.print("CLOSED            ");

    isOpen = 0;

  } else {

    slideWindow();
    lcd.setCursor(2, 1);
    lcd.print("OPENING...             ");
    delay(1000);


    // Open the window
    for (int pos = 0 ; pos <= 35; pos++) {
        distServo.write(pos);
        delay(30);
    }
      
  
    lcd.setCursor(2, 1);
    lcd.print("OPEN              ");

    isOpen = 1;
    }
}


void setup() {
  Serial.begin(9600);

  // Distance sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Distance servo initial state
  distServo.attach(DIST_SERVO);
  distServo.write(0);

  // Temperature servo initial state
  tempServo.attach(TEMP_SERVO);
  tempServo.write(90);

  // Set all the temperature motor control pins to outputs
	pinMode(ENA, OUTPUT);
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	
	// Turn off temperature motor
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);

  //initialise LCD
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("HELLO!             ");
  lcd.setCursor(2, 1);
  lcd.print("CLOSED               ");
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




  // Open and close the window depending on the temperature
  if (temp > TEMP_THRESHOLD) {
    if (!isHalfOpen  && !isOpen)
        slideWindow(); // Slide up the window
  } else {
    if (isHalfOpen)
      slideWindow(); // Slide down the window
  }

  if (temp > VENT_THRESHOLD) { 
    if (!isVentilating)
      ventilateRoom();
  }
  else {
    if (isVentilating)
      ventilateRoom();
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

   // Print to serial
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Open or close the window 
  if (distance < DIST_THRESHOLD) {
      openWindow();
      delay(1000);
  }

  delay(500); // Update every half second
}
