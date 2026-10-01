#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <MPU6050.h>
#include "DHT.h"

// ---------------- PIN CONFIG ----------------
#define DHTPIN 32
#define DHTTYPE DHT11
#define MQ135PIN 34
#define MQ7PIN 35
#define SOSBUTTON 33
#define BUZZER 25

// ---------------- THRESHOLDS ----------------
#define FALL_ANGLE 45
#define MQ135_LIMIT 1200
#define MQ7_LIMIT   800

// ---------------- WIFI MQTT ----------------
const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";
const char* mqtt_server = "BROKER_NAME";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// ---------------- SENSORS ----------------
MPU6050 mpu;
DHT dht(DHTPIN, DHTTYPE);

// ---------------- VARIABLES ----------------
unsigned long lastMsg = 0;
char msg[300];

// SOS Double Click / Long press
unsigned long lastPressTime = 0;
int pressCount = 0;
bool sosActive = false;

// Tone Frequencies
#define TONE_FALL  900
#define TONE_GAS   1500
#define TONE_SOS   400

// Priority Levels
#define PRIORITY_NONE  0
#define PRIORITY_GAS   1
#define PRIORITY_FALL  2
#define PRIORITY_SOS   3
int currentPriority = PRIORITY_NONE;

// Rockfall smoothing
const int ROCKFALL_SMOOTH = 5;
float rockfallBuffer[ROCKFALL_SMOOTH] = {0};
int rockfallIndex = 0;

// ---------------- WIFI SETUP ----------------
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++tries > 40) ESP.restart();
  }
  Serial.println("\nWiFi Connected");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_SafetyVest_Sensor")) {
      Serial.println("connected");
      client.publish("vest/status", "Sensor ESP32 Connected");
    } else {
      delay(2000);
    }
  }
}

// ---------------- TONE FUNCTIONS ----------------
void playTone(int freq, int duration, int count) {
  for (int i = 0; i < count; i++) {
    tone(BUZZER, freq, duration);
    delay(duration + 50); // short pause
  }
  noTone(BUZZER);
}

// ---------------- PRIORITY ALERT HANDLER ----------------
void handleAlert(int p, const char* topic, const char* m, int toneFreq) {
  if (p >= currentPriority) {
    currentPriority = p;
    client.publish(topic, m);
    Serial.println(m);
    playTone(toneFreq, 200, 2);
    currentPriority = PRIORITY_NONE;
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(SOSBUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  Wire.begin();
  mpu.initialize();
  dht.begin();

  Serial.println("ESP32 Sensor Board Ready");
}

// ---------------- MAIN LOOP ----------------
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // ---------- SOS DOUBLE CLICK / LONG PRESS ----------
  if (digitalRead(SOSBUTTON) == LOW) {
    delay(50);
    if (digitalRead(SOSBUTTON) == LOW) {
      unsigned long now = millis();
      if (now - lastPressTime < 500) {
        pressCount++;
      } else {
        pressCount = 1;
      }
      lastPressTime = now;

      // Double press → SOS
      if (pressCount == 2) {
        Serial.println("Double Press → CAMERA CAPTURE");
        client.publish("vest/cam/status", "CAPTURE");
        client.publish("vest/sos", "1"); 
        playTone(TONE_SOS, 100, 4);
        sosActive = true;
        pressCount = 0;
        delay(500);
      }

      // Long press → cancel SOS
      if (pressCount == 1) {
        delay(1500); // long press threshold
        if (digitalRead(SOSBUTTON) == LOW && sosActive) {
          Serial.println("Long Press → CANCEL SOS");
          client.publish("vest/sos", "0");
          sosActive = false;
          pressCount = 0;
        }
      }
    }
  }

  // ----------- SEND SENSOR DATA EVERY 5 SEC -----------
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int mq135 = analogRead(MQ135PIN);
    int mq7 = analogRead(MQ7PIN);

    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    // --- Fall detection ---
    float angle = abs(atan2(ay, az) * 180 / PI);
    bool fallDetected = angle > FALL_ANGLE;

    // --- Rockfall / vibration risk ---
    float gx = ax / 16384.0;
    float gy = ay / 16384.0;
    float gz = az / 16384.0;
    float totalAcc = sqrt(gx*gx + gy*gy + gz*gz) - 1.0;
    if (totalAcc < 0) totalAcc = 0;

    // Smooth rockfall value
    rockfallBuffer[rockfallIndex] = totalAcc;
    rockfallIndex = (rockfallIndex + 1) % ROCKFALL_SMOOTH;
    float smoothedAcc = 0;
    for (int i=0; i<ROCKFALL_SMOOTH; i++) smoothedAcc += rockfallBuffer[i];
    smoothedAcc /= ROCKFALL_SMOOTH;

    int vibrationLevel = min(100, int(smoothedAcc * 100));

    // --- Gas detection ---
    bool gasDetected = (mq135 > MQ135_LIMIT) || (mq7 > MQ7_LIMIT);

    // --- Hardcoded current zone ---
    const char* currentZone = "Zone 1";

    // JSON payload
    snprintf(msg, sizeof(msg),
             "{\"Temperature\":%.2f,\"Humidity\":%.2f,\"MQ135\":%d,\"MQ7\":%d,\"Fall\":%s,\"SOS\":%s,\"MPU6050\":%d,\"Zone\":\"%s\"}",
             temp, hum, mq135, mq7,
             fallDetected ? "true" : "false",
             sosActive ? "true" : "false",
             vibrationLevel,
             currentZone);

    client.publish("vest/data", msg);
    Serial.println(msg);

    // Individual alerts
    if (gasDetected)
      handleAlert(PRIORITY_GAS, "vest/gas", "1", TONE_GAS);

    if (fallDetected)
      handleAlert(PRIORITY_FALL, "vest/fall", "true", TONE_FALL);
  }
}
