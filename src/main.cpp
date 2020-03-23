#include <Arduino.h>
#include <DHT.h>
#include <rgb_lcd.h>
#include "seeed_w600.h"
#include "config.h"

// WiFi
SoftwareSerial softSerial(3, 2);

AtWifi wifi;
String networkName = SSID;
String neworkPassword = PSWD;
char *TARGET_IP = "192.168.1.4";
uint16_t TARGET_PORT = 6900;
uint16_t LOCAL_PORT = 8080;
String msg;
int32_t socket = -1;

// Sensors
DHT sensor(4, DHT11);

// State
int hundredsOfMilis = 0;

// Display
const uint8_t ledStatus = 7;
rgb_lcd lcd;

void displayJust(const char* text) {
  lcd.clear();
  lcd.print(text);
}

void displayError(const char* message) {
  debug.println(message);
  displayJust(message);
  lcd.autoscroll();
}

void displayNetStatus(bool status) {
  lcd.setCursor(14, 1);
  lcd.print(status ? "+" : "-");
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

void setup() {
  debug.begin(9600);

  sensor.begin();
  pinMode(ledStatus, OUTPUT);
  lcd.begin(16, 2);
  lcd.setRGB(255, 0, 0);
  displayJust("Connecting...");

  wifi.begin(softSerial, 9600);
  wifi.wifiReset();
  delay(1000);
  if(!wifi.wifiSetMode(STA)) {
    displayError("Set config mode failed!");
    digitalWrite(7, HIGH);
    return;
  }
  if (!wifi.wifiStaSetTargetApSsid(networkName)) {
    displayError("Set target AP ssid failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(100);
  if (!wifi.wifiStaSetTargetApPswd(neworkPassword)) {
    displayError("Set target AP password failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(100);

  if (!wifi.joinNetwork()) {
    displayError("Join to AP network failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(1500);
  if (!wifi.wifiCreateSocket(msg, TCP, Client, TARGET_IP, TARGET_PORT,
                             LOCAL_PORT)) {
    displayError("Connect to remote server failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(100);
  socket = msg[0] - 0x30;
  debug.print("socket = ");
  debug.println(socket);
  debug.println("Wifi connected, TCP connection established");
  displayJust("Connected");
  delay(2000);

  displayReadingsPlaceholder();
}

void doTheReadings() {
  float hum  = sensor.readHumidity();
  float temp = sensor.readTemperature();

  if (isnan(hum) || isnan(temp) || (hum == 0.0 && temp == 0.0)) {
    displayReadingsPlaceholder();
  } else {
    String data = "0,";
    data += temp;
    data += ",";
    data += hum;
    data += "\n";
    bool netStatus = wifi.wifiSocketSend(msg, socket, data);
    displayReadings(temp, hum);
    displayNetStatus(netStatus);
  }
}

void loop() {
  digitalWrite(7, HIGH);
  delay(100);
  digitalWrite(7, LOW);

  doTheReadings();
  delay(30000);
}
