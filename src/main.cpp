#include <Arduino.h>

// TODO: check if ntp is synced
// TODO: implement RTC

#include <iostream>

#include <ESPAsyncUDP.h>
#include <RtcDS1307.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "SNTP.hpp"

#ifndef SPEED
#define SPEED 115200
#endif

const uint16_t ntpPort = 123;
SNTP sntp = SNTP();
Ticker blinker;
Ticker startAP;
Ticker showTime;
Ticker syncTime;
const char *hostname = "sntp";
const char *APclient = "sntp-config";
const char *APserver = "sntp-server";
const float blink_AP = 1.0;
const float blink_STA = 0.5;
const float blink_nok = 0.1;
AsyncUDP udpServer;
RtcDS1307<TwoWire> Rtc(Wire);

void blink()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void configModeCallback(WiFiManager *myWiFiManager)
{
  blinker.attach(blink_nok, blink);
}

void setRTC()
{
  std::cout << "set RTC" << std::endl;
}

void setTime()
{
  // timeval time;

  std::cout << "set time" << std::endl;

  // time = {std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + sntp.t, 0};
  // settimeofday(&time, nullptr);
}

void receivedUDPServer(AsyncUDPPacket packet)
{
  std::cout << "receive from: " << packet.remoteIP().toString().c_str() << ":" << packet.remotePort() << ", To: " << packet.localIP().toString().c_str() << ":" << packet.localPort() << ", Length: " << packet.length() << std::endl;

  sntp.copy(packet.data());
  sntp.analyze();
}

void handleAP()
{
  syncTime.attach(30, setTime);

  if (WiFi.softAP(APserver) == true)
  {
    blinker.attach(blink_AP, blink);

    if (udpServer.listen(ntpPort))
    {
      std::cout << "UDP Listening on IP: " << WiFi.softAPIP().toString().c_str() << std::endl;
      udpServer.onPacket(receivedUDPServer);
    }
  }
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
    setRTC();
    return true;
  }

  return false;
}

void showClock()
{
  uint32_t timeCountSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  uint32_t timeCountMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  time_t time = timeCountSec;

  std::cout << std::endl
            << "millis: " << millis() << " epoch: " << timeCountMillis << " date: " << ctime(&time);
  std::cout << "offset: " << sntp.getOffset().tv_sec << " " << sntp.getOffset().tv_usec << std::endl;
  std::cout << "delay: " << sntp.getDelay().tv_sec << " " << sntp.getDelay().tv_usec << std::endl;
}

void setupRTC()
{
  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      std::cout << "RTC communications error = " << (uint16_t)Rtc.LastError() << std::endl;
    }
    else
    {
      std::cout << "RTC lost confidence in the DateTime!" << std::endl;
    }
  }

  if (!Rtc.GetIsRunning())
  {
    std::cout << "RTC was not actively running, starting now" << std::endl;
    Rtc.SetIsRunning(true);
  }

  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);
}

void setup()
{
  Serial.begin(SPEED);
  Rtc.Begin();

  pinMode(LED_BUILTIN, OUTPUT);

  setupRTC();

  setTime();
  showClock();

  blinker.attach(blink_nok, blink);
  showTime.attach(30, showClock);

  if (handleSTA())
  {
    startAP.once(10, handleAP);
  }
  else
  {
    handleAP();
  }
}

void loop()
{
}