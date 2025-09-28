// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12    // sonar sensor TRIGGER
#define PIN_ECHO 13    // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0      // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;    // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(115200);
}

void loop() {  
  float distance;
  int ledDuty;
  
  // Apply a sampling interval of 25ms
  if (millis() < (last_sampling_time + INTERVAL)) {
    return;
  }
  last_sampling_time += INTERVAL;
  
  // Read distance from the ultrasonic sensor
  distance = USS_measure(PIN_TRIG, PIN_ECHO);
  
  // Calculate LED brightness based on distance
  if (distance >= 100.0 && distance <= 200.0) {
    // Distance from 100mm to 200mm: brightness increases from min to max
    // Use float for accurate calculation
    ledDuty = (int) ( ( (200.0 - distance) / 100.0 ) * 255.0 );
  } else if (distance > 200.0 && distance <= 300.0) {
    // Distance from 200mm to 300mm: brightness decreases from max to min
    ledDuty = (int) ( ( (distance - 200.0) / 100.0 ) * 255.0 );
  } else if (distance > 300.0) {
    // If distance is greater than 300mm, LED is off
    ledDuty = 255; // LED OFF (active low)
  } else { // If distance is less than 100mm
    // If distance is less than 100mm, LED is off
    ledDuty = 255; // LED OFF (active low)
  }
  
  // Handle special conditions for 150mm and 250mm
  if (distance >= 149.0 && distance <= 151.0) {
    ledDuty = (int)(0.5 * 255); // 50% duty cycle
  }
  if (distance >= 249.0 && distance <= 251.0) {
    ledDuty = (int)(0.5 * 255); // 50% duty cycle
  }

  // Ensure ledDuty is within the valid range [0, 255]
  ledDuty = constrain(ledDuty, 0, 255);

  // Control LED brightness (active low: 0=max brightness, 255=off)
  analogWrite(PIN_LED, ledDuty);
  
  // Output the distance and duty cycle to the serial port
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.print("mm, LED_Duty:");
  Serial.println(ledDuty);
  
  // No delay(50) as it's now controlled by the interval logic
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
