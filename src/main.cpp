#include <Arduino.h>
#include <DHT.h>
#include <rgb_lcd.h>
#include "seeed_w600.h"
#include "config.h"

// WiFi
SoftwareSerial softSerial(PIN_WIFI_RX, PIN_WIFI_TX);

AtWifi wifi;
String msg;
int32_t socket = -1;
bool netStatus = false;
byte netRetry = 0;

// Sensors
DHT tempHumSensor(PIN_TEMP_HUM, DHT11);

// State
int hundredsOfMilis = 0;

// Display
rgb_lcd lcd;
bool displayErrors = true;

void displayJust(const char* text) {
  lcd.clear();
  lcd.print(text);
}

void displayError(const char* message) {
  debug.println(message);
  if (displayErrors) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Blad :(");
    lcd.setCursor(0, 1);
    lcd.print(message);
  }
}

void displayNetStatus() {
  lcd.setCursor(14, 1);
  lcd.print(netStatus ? ":)" : ":(");
}

void displayReadingsPlaceholder() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp. ???`C  net");
  lcd.setCursor(0, 1);
  lcd.print("Wilg. ???%");
}

void displayReadings(int temp, int hum) {
  lcd.setCursor(6, 0);
  lcd.print("   ");
  lcd.setCursor(6, 0);
  lcd.print((int)temp);

  lcd.setCursor(6, 1);
  lcd.print("   ");
  lcd.setCursor(6, 1);
  lcd.print((int)hum);
}

void wifiConnect() {
  String wifiSsid(WIFI_SSID);
  String wifiPass(WIFI_PSWD);
  if (netRetry++ % 4 != 0) {
    return;
  }
  netStatus = false;
  wifi.wifiReset();
  delay(1000);
  if(!wifi.wifiSetMode(STA)) {
    displayError("Set mode failed");
    return;
  }
  if (!wifi.wifiStaSetTargetApSsid(wifiSsid)) {
    displayError("Set net name failed");
    return;
  }
  delay(100);
  if (!wifi.wifiStaSetTargetApPswd(wifiPass)) {
    displayError("Set net pass failed");
    return;
  }
  delay(100);

  if (!wifi.joinNetwork()) {
    displayError("WiFi connect failed");
    return;
  }
  delay(1500);
  if (!wifi.wifiCreateSocket(msg, TCP, Client, COLECTOR_HOST, COLECTOR_PORT, LOCAL_PORT)) {
    displayError("Server connect failed");
    return;
  }
  delay(100);
  socket = msg[0] - 0x30;
  netStatus = true;
  netRetry = 0;
  debug.print("socket = ");
  debug.println(socket);
  debug.println("Wifi connected, TCP connection established");
}

void setup() {
  debug.begin(9600);

  // Initialize display
  tempHumSensor.begin();
  pinMode(PIN_STATUS_LED, OUTPUT);
  lcd.begin(16, 2);
  lcd.setRGB(255, 0, 0);

  // Initialize connection
  displayJust("Connecting...");
  wifi.begin(softSerial, 9600);
  wifiConnect();
  if (netStatus) {
    displayJust("Connected");
  }
  delay(2000);

  displayErrors = false;
  displayReadingsPlaceholder();
}

void doTheReadings() {
  float hum  = tempHumSensor.readHumidity();
  float temp = tempHumSensor.readTemperature();

  if (isnan(hum) || isnan(temp) || (hum == 0.0 && temp == 0.0)) {
    displayReadingsPlaceholder();
  } else {
    String data = "-1,";
    data += temp;
    data += ",";
    data += hum;
    data += "\n";
    netStatus = wifi.wifiSocketSend(msg, socket, data);
    displayReadings(temp, hum);
    displayNetStatus();
  }
}

void loop() {
  digitalWrite(PIN_STATUS_LED, HIGH);
  delay(100);
  digitalWrite(PIN_STATUS_LED, LOW);

  doTheReadings();
  if (netStatus) {
    delay(30000);
  } else {
    wifiConnect();
    displayNetStatus();
    delay(20000);
  }
}
