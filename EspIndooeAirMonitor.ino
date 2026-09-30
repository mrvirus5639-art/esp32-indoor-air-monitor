#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define MP4_AO_PIN 34

const float RL_VALUE = 10.0; 
float R0 = 10.0; 

DHT dht(DHTPIN, DHTTYPE);

float getSensorResistance(float &vOutRead) {
  long mvSum = 0;
  for (int i = 0; i < 32; i++) {
    mvSum += analogReadMilliVolts(MP4_AO_PIN);
    delay(2);
  }
  vOutRead = (mvSum / 32.0) / 1000.0; // Voltage in Volts (0.0V to ~3.1V)
  
  // Clamp vOut within safe math bounds for a 5V-powered MP-4
  float vClamped = constrain(vOutRead, 0.05, 4.90);
  
  // MP-4 is powered by 5.0V (VIN): Rs = RL * (5.0 - Vout) / Vout
  float Rs = RL_VALUE * (5.0 - vClamped) / vClamped;
  return Rs;
}

void setup() {
  Serial.begin(115200);
  delay(500); // Let USB Serial settle after boot
  Serial.println("\n--- ESP32 Booted Successfully ---");

  dht.begin();
  analogReadResolution(12);

  Serial.print("Calibrating MP-4 in clean air: ");
  float rsSum = 0;
  float dummyV = 0;
  for (int i = 10; i > 0; i--) {
    rsSum += getSensorResistance(dummyV);
    Serial.print(i);
    Serial.print(".. ");
    delay(1000);
  }
  R0 = rsSum / 10.0;
  Serial.println("\nCalibration Complete!");
  Serial.print("Baseline R0: ");
  Serial.print(R0, 2);
  Serial.println(" kOhm\n");
}

void loop() {
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  float vOut = 0;
  float rawRs = getSensorResistance(vOut);

  // If DHT11 fails to read, still show MP-4 readings and warn about DHT11
  if (isnan(humidity) || isnan(temp)) {
    Serial.print("[DHT11 Disconnected] | ");
    temp = 25.0;     // Fallback default
    humidity = 55.0; // Fallback default
  } else {
    Serial.print("Temp: "); Serial.print(temp, 1); Serial.print(" °C | ");
    Serial.print("Hum: "); Serial.print(humidity, 0); Serial.print(" % | ");
  }

  // Temperature & Humidity Compensation
  float correction = 1.0 + 0.015 * (temp - 20.0) + 0.008 * (humidity - 55.0);
  float compensatedRs = rawRs * correction;
  float ratio = compensatedRs / R0;

  // In clean air, ratio stays near ~1.00 (Index ~25).
  // When gas/smoke is present, vOut rises -> Rs drops -> ratio drops below 1.0 -> Index rises.
  float gasIndex = constrain((1.0 - ratio) * 300.0 + 25.0, 0.0, 500.0);

  Serial.print("AO Volt: "); Serial.print(vOut, 2); Serial.print("V | ");
  Serial.print("Rs/R0: "); Serial.print(ratio, 2); Serial.print(" | ");
  Serial.print("AQ Index: "); Serial.print(gasIndex, 0);

  if (gasIndex < 60) Serial.println(" (Clean Air)");
  else if (gasIndex < 140) Serial.println(" (Moderate / VOC or Smoke Detected)");
  else Serial.println(" (Poor Air Quality / Gas Detected!)");

  delay(2000);
}