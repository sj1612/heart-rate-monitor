// ====== PIN CONFIG ======
const int IR_PIN  = 34;   // IR photodiode input
const int RED_PIN = 35;   // RED photodiode input

// ====== HEART RATE VARIABLES ======
float alpha = 0.2;      // smoothing factor
float beta  = 0.01;     // baseline tracking

float smoothed = 0;
float baseline = 0;
bool pulseDetected = false;

unsigned long lastBeatTime = 0;
float bpm = 0;

// ====== SpO2 VARIABLES ======
float dc_red = 0, dc_ir = 0;
float ac_red = 0, ac_ir = 0;

float alphaDC = 0.01;
float alphaAC = 0.1;

void setup() {
  Serial.begin(115200);
}

// ====== MAIN LOOP ======
void loop() {

  // ---------- READ SENSORS ----------
  float raw_ir  = analogRead(IR_PIN);
  float raw_red = analogRead(RED_PIN);

  // =========================================================
  // ❤️ HEART RATE FROM IR SIGNAL (same logic as your code)
  // =========================================================

  smoothed = alpha * raw_ir + (1 - alpha) * smoothed;
  baseline = beta  * smoothed + (1 - beta) * baseline;
  float ac_signal = smoothed - baseline;

  float threshold = baseline + 5;  // tune offset

  if (smoothed > threshold && !pulseDetected) {
    pulseDetected = true;

    unsigned long currentTime = millis();
    unsigned long IBI = currentTime - lastBeatTime;
    lastBeatTime = currentTime;

    if (IBI > 300 && IBI < 2000) {
      bpm = 60000.0 / IBI;
      if (bpm > 30 && bpm < 200) {
        Serial.print("BPM: ");
        Serial.println(bpm);
      }
    }
  }

  if (smoothed < baseline) {
    pulseDetected = false;
  }

  // =========================================================
  // 🩸 SpO2 CALCULATION (Ratio-of-Ratios Method)
  // =========================================================

  // --- DC components (baseline) ---
  dc_red = alphaDC * raw_red + (1 - alphaDC) * dc_red;
  dc_ir  = alphaDC * raw_ir  + (1 - alphaDC) * dc_ir;

  // --- AC components (pulsatile) ---
  float red_diff = abs(raw_red - dc_red);
  float ir_diff  = abs(raw_ir  - dc_ir);

  ac_red = alphaAC * red_diff + (1 - alphaAC) * ac_red;
  ac_ir  = alphaAC * ir_diff  + (1 - alphaAC) * ac_ir;

  // --- Ratio-of-Ratios ---
  if (dc_red > 0 && dc_ir > 0 && ac_ir > 0) {
    float R = (ac_red / dc_red) / (ac_ir / dc_ir);

    // Calibration equation
    float SpO2 = 110.0 - 45.0 * R;

    Serial.print("SpO2: ");
    Serial.println(SpO2);
  }

  delay(5); // ~200 Hz sampling
}
