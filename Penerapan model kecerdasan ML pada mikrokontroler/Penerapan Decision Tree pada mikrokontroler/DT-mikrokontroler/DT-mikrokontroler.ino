#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include "time.h"
#include "decision_tree.h"

// ================= WiFi =================
const char* ssid     = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

// ================= OpenWeather =================
const char* OPENWEATHER_API_KEY = "API_KEY_ANDA";
const char* CITY = "Jakarta";
const char* COUNTRY_CODE = "ID";

String weatherURL = "http://api.openweathermap.org/data/2.5/weather?q=" +
                    String(CITY) + "," + String(COUNTRY_CODE) +
                    "&appid=" + String(OPENWEATHER_API_KEY) +
                    "&units=metric";

// ================= NTP (GMT+7) =================
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

// ================= Interval =================
unsigned long interval = 15000;
unsigned long previousMillis = 0;

// ================= Variabel Tambahan =================
float nilai_panjang = 10.5;
float nilai_lebar   = 6.2;

// ================= Decision Tree =================
Eloquent::ML::Port::DecisionTree dt;

// ================= Encode Cuaca =================
int encodeWeatherFromDesc(String weather_desc) {
  weather_desc.toLowerCase();

  if (weather_desc.indexOf("cloud") >= 0) return 1;
  if (weather_desc.indexOf("clear") >= 0) return 2;
  if (weather_desc.indexOf("thunder") >= 0) return 3;
  if (weather_desc.indexOf("haze") >= 0) return 4;
  if (weather_desc.indexOf("rain") >= 0) return 5;

  return 0;
}

// ================= Waktu =================
String getDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "N/A";

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

// ================= OpenWeather (ArduinoJson v5) =================
bool getWeatherData(String &desc) {
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

  desc = doc["weather"][0]["description"].as<String>();
  return true;
}

// ================= Setup =================
void setup() {
  Serial.begin(115200);
  Wire.begin();

  lightMeter.begin();
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Terhubung");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  delay(2000);
}

// ================= Loop =================
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < interval) return;
  previousMillis = currentMillis;

  float lux = lightMeter.readLightLevel();
  float suara = analogRead(SOUND_PIN);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float co2 = analogRead(MQ135_PIN);

  String weather_desc = "unknown";
  getWeatherData(weather_desc);
  int weather_code = encodeWeatherFromDesc(weather_desc);

  Serial.println("===============================");
  Serial.println("Waktu: " + getDateTime());
  Serial.println("Lux: " + String(lux));
  Serial.println("Suara: " + String(suara));
  Serial.println("Temp: " + String(temperature));
  Serial.println("Hum: " + String(humidity));
  Serial.println("CO2: " + String(co2));
  Serial.println("Weather: " + weather_desc);
  Serial.println("Weather Code: " + String(weather_code));

  float input_data[8] = {
    lux,
    suara,
    temperature,
    humidity,
    co2,
    weather_code,
    nilai_panjang,
    nilai_lebar
  };

  int result = dt.predict(input_data);
  Serial.println("Hasil Klasifikasi: " + String(result));
  Serial.println(result == 0 ? "Normal Operation" : "Risk of Failure");
}
