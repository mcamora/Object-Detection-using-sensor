const int TRIG_PIN = 5;    // Trigger pin for HC-SR04
const int ECHO_PIN = 18;   // Echo pin for HC-SR04 (with voltage divider)
const int LED_PIN = 23;    // Red LED for object detection indicator

// Detection threshold and timing
const float DETECTION_THRESHOLD_CM = 15.0;  // Object detected below this distance
const unsigned long MEASURE_INTERVAL = 200; // Measure distance every 200 ms
const unsigned long LED_ON_DURATION = 500;  // LED stays on for 500 ms after detection

// State variables
unsigned long lastMeasureTime = 0;
unsigned long ledOffAt = 0;  // Timestamp when LED should turn off

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("ESP32 Object Detection System");
  Serial.println("HC-SR04 Ultrasonic Sensor");
  Serial.println("Detection threshold: 15 cm");
  Serial.println("----------------------------");
  
  // Configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Ensure trigger pin starts LOW
  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  // Allow sensor to stabilize
  delay(100);
}

void loop() {
  // Non-blocking LED timer: turn off LED after duration expires
  if (ledOffAt != 0 && millis() >= ledOffAt) {
    digitalWrite(LED_PIN, LOW);
    ledOffAt = 0;
  }
  
  // Measure distance at regular intervals (non-blocking)
  if (millis() - lastMeasureTime >= MEASURE_INTERVAL) {
    lastMeasureTime = millis();
    
    // Measure distance using HC-SR04
    float distance = measureDistance();
    
    // Print distance to serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm");
    
    // Check if object is within detection threshold
    if (distance > 0 && distance < DETECTION_THRESHOLD_CM) {
      Serial.println(" - OBJECT DETECTED!");
      
      // Turn on LED for visible feedback (stays on for 500 ms)
      digitalWrite(LED_PIN, HIGH);
      ledOffAt = millis() + LED_ON_DURATION;
    } else {
      Serial.println();
    }
  }
}

/**
 * Measure distance using HC-SR04 ultrasonic sensor
 * Returns distance in centimeters, or -1 if measurement fails
 */
float measureDistance() {
  // Send 10 microsecond pulse to trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure the duration of the echo pulse (timeout after 30ms = ~5m max range)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  // Check for timeout (no echo received)
  if (duration == 0) {
    return -1;  // Invalid reading
  }
  
  // Calculate distance in centimeters
  // Speed of sound is 343 m/s or 0.0343 cm/µs
  // Distance = (duration / 2) * 0.0343
  // Simplified: distance = duration * 0.01715
  float distance = duration * 0.01715;
  
  return distance;
}