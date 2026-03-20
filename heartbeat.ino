
const int SENSOR_PIN = A0;

//Filtering Parameters
float alpha = 0.2;              // EMA smoothing factor
float smoothed = 0;
float prev_smoothed = 0;

//DC Removal (Baseline Tracking)
float baseline = 0;
float beta = 0.01;              // slow baseline update

// Peak Detection
float threshold = 0;
float threshold_offset = 15;    // adjust based on signal

bool pulseDetected = false;

//Heart Rate Calculation
unsigned long currentTime = 0;
unsigned long lastBeatTime = 0;
float IBI = 0;
float bpm = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {

  // Read raw signal
  int raw = analogRead(SENSOR_PIN);

  // EMA Filtering (Low-pass)
  smoothed = alpha * raw + (1 - alpha) * prev_smoothed;
  prev_smoothed = smoothed;

  // DC Removal (Baseline tracking)
  baseline = beta * smoothed + (1 - beta) * baseline;
  float ac_signal = smoothed - baseline;

  // Adaptive Threshold
  threshold = baseline + threshold_offset;

  // Peak Detection (rising edge)
  if (smoothed > threshold && !pulseDetected) {

    currentTime = millis();
    IBI = currentTime - lastBeatTime;

    // Avoid first invalid reading
    if (IBI > 300 && IBI < 2000) {   // valid heart rate range

      bpm = 60000.0 / IBI;

      if (bpm > 30 && bpm < 200) {
        Serial.print("BPM: ");
        Serial.println(bpm);
      }
    }

    lastBeatTime = currentTime;
    pulseDetected = true;
  }

  // Reset detection when signal falls
  if (smoothed < threshold) {
    pulseDetected = false;
  }

  delay(5); // ~200 Hz sampling
}
