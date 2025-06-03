#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "webpage.h"
#include "filters.h"

#define PPG_SENSOR_PIN 27  
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1
#define Highpulse 1900

Adafruit_SSD1306 display(128, 64, &Wire);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
//ButterworthFilter butterworthFilter;
MovingAverageFilter<3> avgFilter; // 10-sample moving average filter
// Example: Cutoff frequency = 2.0 Hz, Sampling rate = 50 Hz
LowPassFilter lowPassFilter(2.0, 50.0);

// WiFi Credentials
const char* ssid = "eir39681112";
const char* password = "7kjmWc93kB";
const char* ssid1 = "Perky";
const char* password1 = "serafin07";

// Heart Icon 16x16px
const unsigned char Heart_Icon [] PROGMEM = {
  0x00, 0x00, 0x18, 0x30, 0x3c, 0x78, 0x7e, 0xfc, 0xff, 0xfe, 0xff, 0xfe, 0xee, 0xee, 0xd5, 0x56, 
  0x7b, 0xbc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00
};

int PulseSensorSignal = 0;
float filteredSensorSignal = 0.0f;
float smoothedSensorSignal = 0.0f;
int timer_Get_BPM = 0;
int cntHB = 0;
boolean ThresholdStat = true;
int BPMval = 0; // Heart rate value
int lastx = 0, lasty = 60, x = 0;
bool get_BPM = true;
long Stime = 0;
long Ltime = 0;

//Setup to sense a nice looking saw tooth on the plotter
byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
byte sampleAverage = 8; //Options: 1, 2, 4, 8, 16, 32
byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
int sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth = 411; //Options: 69, 118, 215, 411
int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
const byte avgAmount = 64;
long baseValue = 0;

const long intervalGetHB = 20;  // 20ms heart rate sampling interval
const long intervalResultHB = 1000;  // 1-second BPM calculation interval
unsigned long previousMillisGetHB = 0;
unsigned long previousMillisResultHB = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    setupOLED();
    //pinMode(PPG_SENSOR_PIN, INPUT);

  for (byte x = 0 ; x < avgAmount ; x++)
  {
    baseValue += analogRead(PPG_SENSOR_PIN); //Read the IR value
  }
  baseValue /= avgAmount;

    connectWiFi();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });
    server.addHandler(&ws);
    server.begin();
     //Pre-populate the plotter so that the Y scale is close to IR values
  for (int x = 0 ; x < 500 ; x++){
    String jsonData = "{\"ppg_value\":" + String((float)baseValue, 2) + "}";
    ws.textAll(jsonData);
    Serial.println(baseValue);
    delay(20);
    }
}

void loop() {
    PulseSensorSignal = analogRead(PPG_SENSOR_PIN);
    Serial.print(PulseSensorSignal);
    Serial.print(",");
    smoothedSensorSignal = avgFilter.process(PulseSensorSignal);
    Serial.println(smoothedSensorSignal);
    loopOLED(smoothedSensorSignal, BPMval);
    BPM(smoothedSensorSignal);
    notifyClients();
    delay(100);
}

void notifyClients() {
    // Send properly formatted JSON
    //String jsonData = "{\"ppg_value\":" + String((float)smoothedSensorSignal, 2) + "}"; 
    //ws.textAll(jsonData);
}

void connectWiFi() {
    Serial.println("Trying to connect to WiFi...");
    WiFi.begin(ssid, password);
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFirst network failed, trying second...");
        WiFi.begin(ssid1, password1);
        startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
            delay(500);
            Serial.print(".");
        }
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected!");
        Serial.print("ESP32 Web Server IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed. Restarting...");
        delay(3000);
        ESP.restart();
    }
}

void BPM(int Svalue) {
    if (Svalue > Highpulse && (millis() - Ltime > 600)) { 
        Stime = millis() - Ltime;
        Ltime = millis();
        BPMval = 60000 / Stime;

        Serial.print("Svalue: ");
        Serial.print(Svalue);
        Serial.print(" | Stime: ");
        Serial.print(Stime);
        Serial.print(" | BPM: ");
        Serial.println(BPMval);
    }
}


void setupOLED() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(37, 0);
    display.print("ESP32");
    display.setCursor(13, 20);
    display.print("HEARTBEAT");
    display.setCursor(7, 40);
    display.print("MONITORING");
    display.display();
    delay(2000);
    display.clearDisplay();
}

void updateGraph(int newValue) {
    int value = map(newValue, 0, 4095, 0, 45); // Map sensor values to OLED graph height
    int y = 40 - value; // Adjust to OLED orientation
    
    // Reset graph if it goes beyond screen width
    if (x > 128) {
        x = 0;
        lastx = 0;
        display.clearDisplay();
    }

    // Draw line from previous point to new point
    display.drawLine(lastx, lasty, x, y, WHITE);

    // Update last drawn positions
    lastx = x;
    lasty = y;
    x++;

    //display.display(); // Show the line on the screen
}


void loopOLED(int pulseValue, int bpm) {
    BPMval = bpm;
    updateGraph(pulseValue);

    // Draw BPM section
    display.fillRect(20, 48, 100, 16, BLACK);
    display.drawBitmap(0, 47, Heart_Icon, 16, 16, WHITE);
    display.drawLine(0, 43, 127, 43, WHITE);
    display.setTextSize(2);
    display.setCursor(23, 48);
    display.setTextColor(SSD1306_WHITE);
    display.print(": ");
    display.print(BPMval);
    display.setCursor(92, 48);
    display.print("BPM");
    display.display();
}


