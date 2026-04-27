
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

// ===== PIN =====
#define FSR A0
#define TRIG 7
#define ECHO 6
#define ENA 5
#define IN1 8
#define IN2 9
#define BUZZER 4

// ===== PARAMETER =====
int fsrThreshold = 100;
int distanceThreshold = 30; // cm (lubang)
int baseSpeed = 55;
int boostSpeed = 80;
float tiltThreshold = 10.0; // degree

// ===== FUNCTION =====
long readDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000); // timeout 30ms
  long distance = duration * 0.034 / 2;
  return distance;
}

float readTilt() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float angle = atan2(ax, az) * 180 / PI;
  return angle;
}

void motorStop() {
  analogWrite(ENA, 0);
}

void motorRun(int speedVal) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, speedVal);
}

// ===== SETUP =====
void setup() {
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Wire.begin();
  mpu.initialize();

  Serial.println("SYSTEM START");
}

// ===== LOOP =====
void loop() {

  int fsrValue = analogRead(FSR);
  long distance = readDistance();
  float tilt = readTilt();

  // ===== SERIAL MONITOR =====
  Serial.print("FSR: "); Serial.print(fsrValue);
  Serial.print(" | Distance: "); Serial.print(distance);
  Serial.print(" cm | Tilt: "); Serial.print(tilt);

  // ===== LOGIK =====

  // 1. TAK SENTUH → STOP
  if (fsrValue < fsrThreshold) {
    motorStop();
    digitalWrite(BUZZER, LOW);
    Serial.println(" | Status: IDLE (tak sentuh)");
  }

  else {

    // 2. LUBANG SEBENAR (ULTRASONIC + TAK CONDONG)
    if (distance > distanceThreshold && abs(tilt) < tiltThreshold) {
      motorStop();
      digitalWrite(BUZZER, HIGH);
      Serial.println(" | Status: LUBANG DETECT → STOP");
    }

    // 3. TANGGA / SLOPE (IGNORE ULTRASONIC)
    else if (abs(tilt) >= tiltThreshold) {
      motorRun(boostSpeed);
      digitalWrite(BUZZER, LOW);
      Serial.println(" | Status: TANGGA/SLOPE → BOOST SPEED");
    }

    // 4. JALAN NORMAL
    else {
      motorRun(baseSpeed);
      digitalWrite(BUZZER, LOW);
      Serial.println(" | Status: JALAN NORMAL");
    }
  }

  delay(200);
}
