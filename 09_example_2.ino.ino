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
                          // Setting EMA to 1 effectively disables EMA filter.

#define N 3               // median filter window size
float buffer[N];
int buf_index = 0;
int buf_count = 0;

// global variables
unsigned long last_sampling_time;   // unit: msec
float dist_prev = _DIST_MAX;        // Distance last-measured
float dist_ema;                     // EMA distance

//  Median 계산 함수
float getMedian() {
  float temp[N];
  for (int i=0; i<buf_count; i++) temp[i] = buffer[i];

  // 삽입 정렬
  for (int i=1; i<buf_count; i++) {
    float key = temp[i];
    int j = i - 1;
    while (j >= 0 && temp[j] > key) {
      temp[j+1] = temp[j];
      j--;
    }
    temp[j+1] = key;
  }

  // 중앙값 반환
  if (buf_count % 2 == 1)
    return temp[buf_count/2];
  else
    return (temp[buf_count/2 - 1] + temp[buf_count/2]) / 2.0;
}

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered, dist_median;
  
  // wait until next sampling time
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

  // 버퍼에 샘플 저장
  buffer[buf_index] = dist_raw;
  buf_index = (buf_index + 1) % N;
  if (buf_count < N) buf_count++;

  // median 계산
  dist_median = getMedian();

  // range filter
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      dist_filtered = dist_prev;
  } else if (dist_raw < _DIST_MIN) {
      dist_filtered = dist_prev;
  } else {    // In desired Range
      dist_filtered = dist_raw;
      dist_prev = dist_raw;
  }

  // EMA 적용
  dist_ema = _EMA_ALPHA*dist_filtered + (1-_EMA_ALPHA)*dist_ema; 

  // output to serial
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",raw:"); Serial.print(dist_raw);
  Serial.print(",median:");  Serial.print(dist_median);
  Serial.print(",filtered:");  Serial.print(dist_filtered);
  Serial.print(",ema:");  Serial.print(dist_ema);
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");

  // LED control
  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, 1);       // LED OFF
  else
    digitalWrite(PIN_LED, 0);       // LED ON

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}
