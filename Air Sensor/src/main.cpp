#include <Arduino.h>
#include <SoftwareSerial.h> 
#include <WiFi.h>
#include <esp_now.h>
#include "CO2Sensor.h"
#include "DHT.h"

SoftwareSerial mySerial(21, 22); 
CO2Sensor co2Sensor(33, 0.90, 100);
DHT dht(32, DHT22);
struct pm
{
  uint32_t pm1 = 0;
  uint32_t pm2_5 = 0;
  uint32_t pm10 = 0;
  uint32_t co2 = 0;
  uint32_t temp = 0;
  uint32_t hum = 0;
};
pm pm_readings;
esp_now_peer_info_t peerInfo;
uint8_t broadcastAddress[] = {0xc0, 0x49, 0xef, 0xf9, 0x93, 0xd4};
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void SetUpESPNow()
{
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void ReadPMS3003(void)
{
  int index = 0;
  char value;
  char previousValue;

  while (mySerial.available()) 
  {
    value = mySerial.read();                                            
    if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)) 
    {
      Serial.println("Cannot find the data header.");
      break;
    }

    if (index == 10 || index == 12 || index == 14)
    {
      previousValue = value;
    }
    else if (index == 11)
    {
      pm_readings.pm1 = 256 * previousValue + value; 
      Serial.print("{ ");
      Serial.print("\"pm1\": ");
      Serial.print(pm_readings.pm1);
      Serial.print(" ug/m3");
      Serial.print(", ");
    }
    else if (index == 13)
    {
      pm_readings.pm2_5 = 256 * previousValue + value;
      Serial.print("\"pm2_5\": ");
      Serial.print(pm_readings.pm2_5);
      Serial.print(" ug/m3");
      Serial.print(", ");
    }
    else if (index == 15)
    {
      pm_readings.pm10 = 256 * previousValue + value;
      Serial.print("\"pm10\": ");
      Serial.print(pm_readings.pm10);
      Serial.print(" ug/m3");
    }
    else if (index > 15)
    {
      break;
    }
    index++;
  }
  while (mySerial.available())
    mySerial.read();
  Serial.println(" }");
}
unsigned long previousMillis = 0;
const long interval = 1000;

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  mySerial.begin(9600);

  SetUpESPNow();
  pinMode(33, INPUT);
  co2Sensor.calibrate();
  dht.begin();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;
    ReadPMS3003();
    pm_readings.co2 = co2Sensor.read();
    Serial.printf("CO2 value: %d ppm\n", pm_readings.co2);
    pm_readings.temp = dht.readTemperature();
    Serial.printf("Temperature: %d °C\n", pm_readings.temp);
    pm_readings.hum = dht.readHumidity();
    Serial.printf("Humidity: %d \n", pm_readings.hum);
    esp_now_send(broadcastAddress, (uint8_t *)&pm_readings, sizeof(pm_readings));
  }
}
