#include <Arduino.h>
#include <DHT.h>
#include "seeed_w600.h"
#include "config.h"

SoftwareSerial softSerial(3, 2);

AtWifi wifi;
String networkName = SSID;
String neworkPassword = PSWD;
char *TARGET_IP = "192.168.1.4";
uint16_t TARGET_PORT = 6900;
uint16_t LOCAL_PORT = 8080;
String msg;
int32_t socket = -1;

DHT sensor(4, DHT11);
const uint8_t ledStatus = 7;

void setup() {
  sensor.begin();
  pinMode(ledStatus, OUTPUT);
  debug.begin(9600);
  debug.println("Zaczynamy");

  wifi.begin(softSerial, 9600);
  wifi.wifiReset();
  delay(1000);
  if(!wifi.wifiSetMode(STA)) {
    debug.println("Set config mode failed!");
    digitalWrite(7, HIGH);
    return;
  }
  if (!wifi.wifiStaSetTargetApSsid(networkName)) {
    debug.println("Set target AP ssid failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(100);
  if (!wifi.wifiStaSetTargetApPswd(neworkPassword)) {
    debug.println("Set target AP password failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(100);

  if (!wifi.joinNetwork()) {
    debug.println("Join to AP network failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(1500);
  if (!wifi.wifiCreateSocket(msg, TCP, Client, TARGET_IP, TARGET_PORT,
                             LOCAL_PORT)) {
    debug.println("Connect to remote server failed!!");
    digitalWrite(7, HIGH);
    return;
  }
  delay(100);
  socket = msg[0] - 0x30;
  debug.print("socket = ");
  debug.println(socket);
  debug.println("Wifi connected, TCP connection established");
}

void loop() {
  float hum  = sensor.readHumidity();
  float temp = sensor.readTemperature();

  if (isnan(hum) || isnan(temp) || (hum == 0.0 && temp == 0.0)) {
    digitalWrite(7, HIGH);
  } else {
    digitalWrite(7, LOW);
    String data = "0,";
    data += temp;
    data += ",";
    data += hum;
    data += "\n";
    wifi.wifiSocketSend(msg, socket, data);
    // Serial.print((int)hum);
    // Serial.print(",");
    // Serial.print((int)temp);
    // Serial.println();
  }

  delay(30000);
}
