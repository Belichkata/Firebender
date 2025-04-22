#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>

// Create an instance of the accelerometer
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Punch detection settings for HARD punches
unsigned long punchStart = 0;
unsigned long lastPunchTime = 0;
const long punchInterval = 200;     // Time between accel & decel
const long punchCooldown = 2000;    // 2 seconds between punches
float punchAccel = 6.0;             // Require ~6G acceleration
float punchDecel = -8.0;            // Require ~-8G deceleration
int flameTime = 250;                // Flame duration in milliseconds

void setup() {
  Serial.begin(9600);
  Serial.println("ADXL345 test!");

  if (!accel.begin()) {
    Serial.println("Could not find ADXL345. Check wiring!");
    while (1);
  }
  Serial.println("ADXL345 found!");

  // Set range to ±16G for punch detection
  accel.setRange(ADXL345_RANGE_16_G);

  pinMode(8, OUTPUT); // Solenoid valve
  pinMode(9, OUTPUT); // Arc lighter
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  unsigned long currentMillis = millis();

  // Detect punch acceleration if cooldown period has passed
  if (event.acceleration.x > punchAccel && currentMillis - lastPunchTime > punchCooldown) {
    punchStart = currentMillis;
  }

  // Detect punch deceleration within allowed interval
  if (event.acceleration.x < punchDecel &&
      currentMillis - punchStart < punchInterval &&
      currentMillis - lastPunchTime > punchCooldown) {
    
    //Serial.println("🔥 HARD PUNCH DETECTED!");
    Fire(flameTime);
    lastPunchTime = currentMillis;
  }

  delay(50); // Short delay to prevent too frequent readings
}

void Fire(int flameTime) {
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  delay(flameTime);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
}
