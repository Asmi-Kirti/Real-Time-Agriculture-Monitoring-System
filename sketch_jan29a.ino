#define BLYNK_TEMPLATE_ID "TMPL3tenOCYa5"
#define BLYNK_TEMPLATE_NAME "Real time agricultural monitoring system"

//Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = "V0yUbI0IN5wKocQXLPl9-9bxk1RSfFYI";  //Enter your Blynk Auth token
char ssid[] = "Asmi";  //Enter your WIFI SSID
char pass[] = "asmi1804   ";  //Enter your WIFI Password

DHT dht(D3, DHT11); //(DHT sensor pin, sensor type) D4 DHT11 Temperature Sensor
BlynkTimer timer;

// Define component pins
#define SOIL_SENSOR A0  // Soil Moisture Sensor
#define RAIN_SENSOR D2  // Rain Sensor
#define RELAY_PIN D5    // Relay for water pump

void setup() {
  Serial.begin(9600);
  pinMode(SOIL_SENSOR, INPUT);
  pinMode(RAIN_SENSOR, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  //Call functions at intervals
  timer.setInterval(1000L, readDHT11);
  timer.setInterval(1000L, checkSoilMoisture);
  timer.setInterval(1000L, checkRainSensor);
}

// Read DHT11 sensor values
void readDHT11() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

// Check soil moisture and control water pump
void checkSoilMoisture() {
  int soilValue = analogRead(SOIL_SENSOR);
  soilValue = map(soilValue, 0, 1024, 0, 100);
  soilValue = (soilValue - 100) * -1;
  Blynk.virtualWrite(V3, soilValue);
}

// Check rain sensor and control relay
void checkRainSensor() {
  int rainValue = digitalRead(RAIN_SENSOR);
  Blynk.virtualWrite(V4, rainValue);

  int soilValue = analogRead(SOIL_SENSOR);
  soilValue = map(soilValue, 0, 1024, 0, 100);
  soilValue = (soilValue - 100) * -1;
  
  if (soilValue < 40 && rainValue == 0) { // If soil is dry and no rain
    digitalWrite(RELAY_PIN, HIGH); // Turn on pump
    Blynk.virtualWrite(V5, 1);
  } else {
    digitalWrite(RELAY_PIN, LOW); // Turn off pump
    Blynk.virtualWrite(V5, 0);
  }
}

void loop() {
  Blynk.run();
  timer.run();
}