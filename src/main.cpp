#include <Arduino.h>

#include <iostream>

#include <ESPAsyncUDP.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "SNTP.hpp"

#ifndef SPEED
#define SPEED 115200
#endif

SNTP sntp = SNTP();
Ticker blinker;
const char *hostname = "sntp";
const float blink_ok = 0.5;
const float blink_nok = 0.1;
AsyncUDP udpServer;
AsyncUDP udpClient;

void blink()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void setupWiFi()
{
  WiFiManager wifiManager;

  WiFi.mode(WIFI_STA);

  wifiManager.setTimeout(180);
  wifiManager.setConnectTimeout(10);

  WiFi.hostname(hostname);

  if (!wifiManager.autoConnect(hostname))
  {
    ESP.restart();
  }
}

void receivedUDP(AsyncUDPPacket packet)
{
  Serial.print("UDP Packet Type: ");
  Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
  Serial.print(", From: ");
  Serial.print(packet.remoteIP());
  Serial.print(":");
  Serial.print(packet.remotePort());
  Serial.print(", To: ");
  Serial.print(packet.localIP());
  Serial.print(":");
  Serial.print(packet.localPort());
  Serial.print(", Length: ");
  Serial.print(packet.length());
  Serial.print(", Data: ");
  Serial.write(packet.data(), packet.length());
  Serial.println();
  //reply to the client
  packet.printf("Got %u bytes of data", packet.length());
}

void sendUDP(IPAddress ip, uint16_t port, std::string message)
{
  if (udpClient.connect(ip, port))
  {
    Serial.println("UDP connected");
    //Send unicast
    udpClient.print(message.c_str());
  }
}

void setup()
{
  Serial.begin(SPEED);

  pinMode(LED_BUILTIN, OUTPUT);

  blinker.attach(blink_nok, blink);
  setupWiFi();
  blinker.attach(blink_ok, blink);

  if (udpServer.listen(1234))
  {
    std::cout << "UDP Listening on IP: " << WiFi.localIP().toString().c_str() << std::endl;
    udpServer.onPacket(receivedUDP);
  }

  delay(1000);
  sntp.clientPacketPrepare();

  sntp.printDate();

  delay(5000);
  sntp.clientPacketPrepare();

  sntp.printDate();

  delay(5000);
  sntp.clientPacketPrepare();

  sntp.printDate();
}

void loop()
{
  delay(15000);
  sendUDP(IPAddress(192, 168, 178, 47), 1234, "hello hello 1234");
  sendUDP(IPAddress(192, 168, 178, 47), 1235, "hello hello 1235");
}