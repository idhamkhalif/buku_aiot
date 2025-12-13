
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

const char* ssid = "Cem-Cem";
const char* password = "Kingston123";

const char* WEB_APP_URL = "https://script.google.com/macros/s/AKfycbwUrUOpb48TFKFgoqHJnIanWhN8vQm02mfzEOTveEKjirgcM_ht0GrG5ftiaRYbb6_5qw/exec";

// ==== NTP (WIB / GMT+7) ====
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;  // GMT+7 (WIB)
const int daylightOffset_sec = 0;

unsigned long interval = 15000; // setiap 15 detik
unsigned long previousMillis = 0;

String getDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("❌ Gagal mendapatkan waktu NTP");
    return "N/A";
  }

  char timeStringBuff[30];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Terhubung ke WiFi!");

  // Sinkronisasi waktu NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sinkronisasi waktu NTP...");
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // ==== Data Sensor (dummy untuk contoh) ====
      float lux = random(0,100);
      float suara = random(0,100);
      float temperature = random(20,35);
      float humidity = random(70,100);
      float co2 = random(100, 400);
      String datetime = getDateTime();  // Ambil waktu dari NTP server

      // ==== Buat JSON Payload (ArduinoJson v5) ====
      StaticJsonBuffer<300> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();

      root["lux"] = lux;
      root["suara"] = suara;
      root["temperature"] = temperature;
      root["humidity"] = humidity;
      root["co2"] = co2;
      root["datetime"] = datetime;

      String requestBody;
      root.printTo(requestBody);

      // ==== Kirim Data ke Google Apps Script ====
      http.begin(WEB_APP_URL);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(requestBody);

      if (httpResponseCode > 0) {
        Serial.printf("✅ Data terkirim! Code: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println("Respon: " + response);
        Serial.println("Waktu dikirim: " + datetime);
      } else {
        Serial.printf("Gagal kirim data. Code: %d\n", httpResponseCode);
      }

      http.end();
    } else {
      Serial.println("WiFi terputus, mencoba menyambung kembali...");
      WiFi.begin(ssid, password);
    }
  }

}
