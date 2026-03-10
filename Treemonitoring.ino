#define BLYNK_TEMPLATE_ID "TMPL35GOS6_uS"
#define BLYNK_TEMPLATE_NAME "treemonitoring"
#define BLYNK_AUTH_TOKEN "ajJ8bYxhjTFSXs4XBR4YdXVdQOlgUwnj"

#define BLYNK_PRINT Serial
#define BLYNK_DEBUG

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "";
const char* pass = ""; // 🔐 Add password if your hotspot has one

// Pin definitions
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135_PIN 34
#define SOIL_PIN 35

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("[DHT] Failed to read from DHT sensor!");
    return;
  }

  int airQuality = analogRead(MQ135_PIN);
  int soilMoisture = analogRead(SOIL_PIN);

  Serial.println("------ Sensor Readings ------");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Air Quality: ");
  Serial.print(airQuality);
  Serial.println(" AQI");

  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);

  Blynk.virtualWrite(V0, airQuality);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, soilMoisture);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  wifi_attempts++;
  if (wifi_attempts > 10) {
    Serial.println("[WiFi] Connection failed. Restarting...");
    ESP.restart();
    }
  }

  Serial.println("\n[WiFi] Connected!");
  Serial.print("[WiFi] IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();

  Blynk.config(BLYNK_AUTH_TOKEN);
  if (!Blynk.connect(10000)) {
    Serial.println("[Blynk] Connection timeout. Restarting...");
    ESP.restart();
  }

  timer.setInterval(2000L, sendSensorData); // Read sensors every 2 sec
}

void loop() {
  Blynk.run();
  timer.run();
}
