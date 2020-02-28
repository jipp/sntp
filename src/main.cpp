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

const uint16_t ntpPort = 123;
const char *ntpServerName = "0.de.pool.ntp.org";
// const char *ntpServerName = "fritz.box";
SNTP sntp = SNTP();
Ticker blinker;
Ticker NTPclient;
Ticker NTPserver;
const char *hostname = "sntp";
const char *APclient = "sntp-config";
const char *APserver = "sntp-server";
const float blink_AP = 1.0;
const float blink_STA = 0.5;
const float blink_nok = 0.1;
AsyncUDP udpServer;
AsyncUDP udpClient;

void blink()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void configModeCallback(WiFiManager *myWiFiManager)
{
  blinker.attach(blink_nok, blink);
}

void receivedUDPServer(AsyncUDPPacket packet)
{
  std::cout << "receive from: " << packet.remoteIP().toString().c_str() << ":" << packet.remotePort() << ", To: " << packet.localIP().toString().c_str() << ":" << packet.localPort() << ", Length: " << packet.length() << std::endl;

  sntp.copy(packet.data());
  sntp.analyze();
}

void handleAP()
{
  if (WiFi.softAP(APserver) == true)
  {
    blinker.attach(blink_AP, blink);
    NTPclient.detach();

    if (udpServer.listen(ntpPort))
    {
      std::cout << "UDP Listening on IP: " << WiFi.softAPIP().toString().c_str() << std::endl;
      udpServer.onPacket(receivedUDPServer);
    }
  }
}

void receivedUDPClient(AsyncUDPPacket packet)
{
  std::cout << "receive from: " << packet.remoteIP().toString().c_str() << ":" << packet.remotePort() << ", To: " << packet.localIP().toString().c_str() << ":" << packet.localPort() << ", Length: " << packet.length() << std::endl;

  sntp.copy(packet.data());
  sntp.analyze();
  NTPclient.detach();
  NTPserver.detach();
  handleAP();

  std::cout << "offset: " << sntp.t << " delay: " << sntp.d << std::endl;
}

boolean handleSTA()
{
  WiFiManager wifiManager;

  WiFi.mode(WIFI_STA);
  blinker.attach(blink_STA, blink);

  wifiManager.setConfigPortalTimeout(180);
  wifiManager.setConnectTimeout(10);
  wifiManager.setAPCallback(configModeCallback);
  // wifiManager.resetSettings();

  WiFi.hostname(hostname);

  if (wifiManager.autoConnect(APclient))
  {
    return true;
  }

  return false;
}

void ntpSync()
{
  IPAddress ntpServerIP;

  if (WiFi.hostByName(ntpServerName, ntpServerIP) == 1)
  {
    udpClient.onPacket(receivedUDPClient);
    sntp.prepareClient();

    if (udpClient.writeTo((uint8_t *)&sntp.packet, sizeof(sntp.packet), ntpServerIP, ntpPort))
    {
      std::cout << "send packet to: " << ntpServerIP.toString().c_str() << std::endl;
    }
  }
}

void setup()
{
  Serial.begin(SPEED);

  pinMode(LED_BUILTIN, OUTPUT);
  blinker.attach(blink_nok, blink);

  if (handleSTA())
  {
    NTPclient.attach(1, ntpSync);
    NTPserver.once(10, handleAP);
  }
  else
  {
    handleAP();
  }
}

void loop()
{
}