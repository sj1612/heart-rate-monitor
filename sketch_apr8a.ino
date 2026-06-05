#include "arduinoFFT.h"
#include "LiquidCrystal.h"

#define SENSOR_PIN A0

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const float ema_factor = 0.5;
const int smudge_width = 2;

const uint16_t samples = 128;
const float sample_frequency = 50.0;
// Make these unsigned long to handle the massive numbers of micros()
const unsigned long sample_period = 1000000 / sample_frequency; 
unsigned long next_target_time;

float vReal[samples];
float vImag[samples];

ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, samples, sample_frequency);

float ema_smooth_value;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  
  ema_smooth_value = analogRead(SENSOR_PIN);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Hello :0");
  // lcd.setCursor(0, 1);
  // lcd.print("Xeramian :3");
  
  delay(2000);
}

void loop() {
  next_target_time = micros();
  
  // ==========================================
  // GATHERING PHASE (Bug 1 & 2 Fixed)
  // ==========================================
  for (int i = 0; i < samples; i++) {
    int rawValue = analogRead(SENSOR_PIN);
    
    // 1. Calculate the smoothed value
    ema_smooth_value = ema_factor * rawValue + (1.0 - ema_factor) * ema_smooth_value;
    
    // 2. Save the smoothed value to the array
    vReal[i] = ema_smooth_value;
    
    // 3. Reset the imaginary array every time!
    vImag[i] = 0.0; 
    
    next_target_time += sample_period;
    while (micros() < next_target_time);
  }

  // ==========================================
  // FFT MATH PHASE
  // ==========================================
  FFT.dcRemoval();
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  float max_mag = 0;
  int max_bin = 0;

  for (int i = 2; i <= 10; i++) {
    if (vReal[i] > max_mag) {
      max_mag = vReal[i];
      max_bin = i; 
    }
  }

  // ==========================================
  // HARMONIC & CENTROID PHASE (Bug 3 Fixed)
  // ==========================================
  int true_bin = max_bin;
  int half_bin = max_bin / 2;

  // Proper Harmonic Check: Is the half-bin strong enough to be the true beat?
  if (half_bin >= 2 && vReal[half_bin] > (max_mag * 0.30)) {
    true_bin = half_bin;
  }

  float weighted_sum = 0.0;
  float total_mag = 0.0;
  
  // Draw the "smudge" boundaries strictly around the verified true_bin
  int start_bin = max(2, true_bin - smudge_width);
  int end_bin = min(10, true_bin + smudge_width);

  for (int i = start_bin; i <= end_bin; i++) {
    float binFreq = i * (sample_frequency / samples);
    weighted_sum += (binFreq * vReal[i]);
    total_mag += vReal[i];
  }

  // ==========================================
  // OUTPUT PHASE
  // ==========================================
  Serial.print("Mag: ");
  Serial.println(total_mag);

  lcd.clear(); 

  if (total_mag > 80.0) { 
    
    float heart_beat_freq = weighted_sum / total_mag;
    int bpm = round(heart_beat_freq * 60.0);
    
    Serial.print("True Freq: ");
    Serial.print(heart_beat_freq);
    Serial.print(" Hz  |  BPM: ");
    Serial.println(bpm);
    
    lcd.setCursor(0, 0);
    lcd.print("Heart Rate:");
    lcd.setCursor(0, 1); 
    lcd.print(bpm);
    lcd.print(" BPM");
    
  } else {
    Serial.println("Signal too weak...");
    lcd.print("Reading Sensor");
    lcd.setCursor(0, 1);
    lcd.print("Please wait...");
  }
}