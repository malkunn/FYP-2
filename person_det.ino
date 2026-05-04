const int TRIG_PIN = 8;
const int ECHO_PIN = 9;
const int BUZZER_PIN = 13;

const int BUTTON_PIN = 7;        // tactile button
const int FALL_THRESHOLD_CM = 90;

bool systemOn = false;
bool lastButton = HIGH;

long readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (duration == 0) return 999;
  return (long)(duration / 58);
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // IMPORTANT

  Serial.begin(115200);
}

void loop() {
  // ---- Button toggle (with simple debounce) ----
  bool buttonNow = digitalRead(BUTTON_PIN);

  // Detect press (HIGH -> LOW)
  if (lastButton == HIGH && buttonNow == LOW) {
    systemOn = !systemOn;     // toggle ON/OFF
    delay(200);               // debounce delay
  }
  lastButton = buttonNow;

  // ---- System behavior ----
  if (!systemOn) {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println(0);        // plot shows 0 when OFF
    delay(100);
    return;
  }

  long distance = readDistanceCM();
  bool fallDetected = (distance >= FALL_THRESHOLD_CM);

  digitalWrite(BUZZER_PIN, fallDetected ? HIGH : LOW);

  // Plot distance vs time
  Serial.println(distance);

  delay(100);
}
