// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.1    // EMA weight of new sample (range: 0 to 1)

#define N 30              // median filter window size
float buffer[N];
int buf_index = 0;
int buf_count = 0;

// global variables
unsigned long last_sampling_time;   // unit: msec
float dist_ema;                     // EMA distance

float getMedian() {
  float temp[N];
  for (int i = 0; i < buf_count; i++) temp[i] = buffer[i];

  // 삽입 정렬
  for (int i = 1; i < buf_count; i++) {
    float key = temp[i];
    int j = i - 1;
    while (j >= 0 && temp[j] > key) {
      temp[j + 1] = temp[j];
      j--;
    }
    temp[j + 1] = key;
  }

  // 중앙값 반환
  if (buf_count % 2 == 1)
    return temp[buf_count / 2];
  else
    return (temp[buf_count / 2 - 1] + temp[buf_count / 2]) / 2.0;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_median;

  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  buffer[buf_index] = dist_raw;
  buf_index = (buf_index + 1) % N;
  if (buf_count < N) buf_count++;

  dist_median = getMedian();

  dist_ema = _EMA_ALPHA * dist_median + (1 - _EMA_ALPHA) * dist_ema;

  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",raw:");   Serial.print(dist_raw);
  Serial.print(",ema:");   Serial.print(dist_ema);
  Serial.print(",median:");Serial.print(dist_median);
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");

  if ((dist_median < _DIST_MIN) || (dist_median > _DIST_MAX))
    digitalWrite(PIN_LED, 1);   // LED OFF
  else
    digitalWrite(PIN_LED, 0);   // LED ON

  last_sampling_time += INTERVAL;
}
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // mm 단위로 변환
}
