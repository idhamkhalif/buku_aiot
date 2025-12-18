#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include "time.h"

// =================== WiFi ===================
const char* ssid = "SSID";
const char* password = "SSID_PASSWORD";

// =================== Google Apps Script ===================
const char* WEB_APP_URL =
"https://script.google.com/macros/s/AKfycbwUrUOpb48TFKFgoqHJnIanWhN8vQm02mfzEOTveEKjirgcM_ht0GrG5ftiaRYbb6_5qw/exec";

// =================== OpenWeather ===================
const char* OPENWEATHER_API_KEY = "API_KEY_ANDA";
const char* CITY = "Jakarta";
const char* COUNTRY_CODE = "ID";

String weatherURL = "http://api.openweathermap.org/data/2.5/weather?q=" +
                    String(CITY) + "," + String(COUNTRY_CODE) +
                    "&appid=" + String(OPENWEATHER_API_KEY) +
                    "&units=metric";

// =================== NTP (GMT+7) ===================
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

// ================= Sensor =================
#define DHTPIN 4
#define DHTTYPE DHT11
#define SOUND_PIN 34
#define MQ135_PIN 35

BH1750 lightMeter;
DHT dht(DHTPIN, DHTTYPE);

// ================= Variabel Tambahan =================
float nilai_panjang = 10.5;
float nilai_lebar   = 6.2;

// ================= Interval =================
unsigned long interval = 15000;
unsigned long previousMillis = 0;

// ================= Fungsi Waktu =================
String getDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "N/A";

  char timeStringBuff[30];
  strftime(timeStringBuff, sizeof(timeStringBuff),
           "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

// ================= Fungsi OpenWeather =================
bool getWeatherData(float &temp_out, float &hum_out, String &desc) {
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  http.begin(weatherURL);

  int httpCode = http.GET();
  if (httpCode <= 0) {
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  StaticJsonBuffer<600> jsonBuffer;
  JsonObject& doc = jsonBuffer.parseObject(payload);
  if (!doc.success()) return false;

  desc     = doc["weather"][0]["description"].as<String>();

  return true;
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Wire.begin();

  lightMeter.begin();
  dht.begin();

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Terhubung");

  // NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Sinkronisasi waktu...");
  delay(2000);
}

// ================= LOOP =================
void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < interval) return;
  previousMillis = currentMillis;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi terputus, reconnect...");
    WiFi.begin(ssid, password);
    return;
  }

  HTTPClient http;

  // ===== Sensor Lokal =====
  float lux = lightMeter.readLightLevel();
  float suara = analogRead(SOUND_PIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float co2 = analogRead(MQ135_PIN);

  String datetime = getDateTime();

  // ===== OpenWeather =====
  float temp_out = 0, hum_out = 0;
  String weather_desc = "N/A";
  bool weatherOK = getWeatherData(temp_out, hum_out, weather_desc);

  // ===== Debug Serial =====
  Serial.println("===============================");
  Serial.println("Data Sensor & Cuaca:");
  Serial.println("Lux: " + String(lux));
  Serial.println("Suhu Dalam: " + String(temperature));
  Serial.println("Humidity Dalam: " + String(humidity));
  Serial.println("CO2: " + String(co2));
  Serial.println("Suhu Luar: " + String(temp_out));
  Serial.println("Humidity Luar: " + String(hum_out));
  Serial.println("Cuaca: " + weather_desc);

  // ===== JSON =====
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["lux"] = lux;
  root["suara"] = suara;
  root["temperature"] = temperature;
  root["humidity"] = humidity;
  root["co2"] = co2;
  root["cuaca"] = weather_desc;
  root["panjang"] = nilai_panjang;
  root["lebar"] = nilai_lebar;
  root["datetime"] = datetime;

  String requestBody;
  root.printTo(requestBody);

  // ===== Kirim ke Google Script =====
  http.begin(WEB_APP_URL);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(requestBody);

  if (httpCode > 0) {
    Serial.println("✅ Data terkirim");
    Serial.println(http.getString());
  } else {
    Serial.println("❌ Gagal kirim data");
  }

  http.end();
}
