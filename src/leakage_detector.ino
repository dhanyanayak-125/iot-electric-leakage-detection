#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Initialize 16x2 LCD display at standard I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Hardware & Network Configuration
const int sensorPin = 34; 
const float VREF = 3.3;
const char* ssid = "Dhanu";
const char* password = "12345678";

// ThingSpeak Telemetry Configurations
String apiKey = "12LOTWD4T9RU9H0L"; 
String server = "http://thingspeak.com";

// ===== TRUE AC RMS CALIBRATION CALCULATOR =====
float getCurrent() {
    const int samples = 600;
    long sum = 0;
    
    // Establish steady state DC offset via block sampling
    for (int i = 0; i < samples; i++) { 
        sum += analogRead(sensorPin);
    }
    float offset = sum / samples;
    
    // Compute Root-Mean-Square (RMS) across alternative intervals
    long sumSq = 0;
    for (int i = 0; i < samples; i++) { 
        float raw = analogRead(sensorPin) - offset; 
        sumSq += raw * raw;
    }
    
    float meanSq = (float)sumSq / samples;
    float rootMeanSq = sqrt(meanSq);
    
    // Convert ADC steps back to structural analog voltage metrics
    float voltageRMS = (rootMeanSq / 4095.0) * VREF;
    
    // SCT-013 Calibration factor (proportional conversion ratio)
    float calculatedCurrent = voltageRMS * 30.0; 
    return calculatedCurrent;
}

// ===== SETUP GATEWAYS =====
void setup() {
    Serial.begin(115200);
    
    // Initialize custom hardware I2C pins for ESP32
    Wire.begin(21, 22); 
    lcd.init();
    lcd.backlight();
    
    // Configure ESP32 ADC resolution to 12-Bit (0 - 4095 steps)
    analogReadResolution(12);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CT Monitor Ready");
    
    // Establish Local Wi-Fi Handshake Gateway
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    delay(1000);
}

// ===== MAIN CONTINUOUS REFRESH LOOP =====
void loop() {
    // 1. Gather current value using signal conditioning algorithms
    float current = getCurrent();
    
    // 2. Output data stream locally on 16x2 I2C Display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current: ");
    lcd.print(current, 2);
    lcd.print("A");
    
    // 3. Threshold Evaluation & Safety State Matrix
    lcd.setCursor(0, 1);
    if (current > 5.0) { 
        lcd.print("STATUS: DANGER!");
        Serial.println("[ALERT] Leakage threshold breached!");
    } else { 
        lcd.print("STATUS: SAFE ");
    }
    
    // 4. Pump Real-Time Field Streams to ThingSpeak Cloud Analytics
    if (WiFi.status() == WL_CONNECTED) { 
        HTTPClient http; 
        String url = server + "?api_key=" + apiKey + "&field1=" + String(current, 2); 
        
        http.begin(url); 
        int httpCode = http.GET(); 
        
        Serial.print("ThingSpeak Response: "); 
        Serial.println(httpCode); 
        http.end(); 
    }
    
    // 20-second data logging lock to comply with standard ThingSpeak intervals
    delay(20000); 
}
