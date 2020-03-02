#include <Arduino.h>

// TODO: check if ntp is synced

#include <iostream>

#include <ESPAsyncUDP.h>
#include <RtcDS1307.h>
#include <Ticker.h>
#include <time.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "SNTP.hpp"

#ifndef SPEED
#define SPEED 115200
#endif

SNTP sntp = SNTP();
const uint16_t ntpPort = 123;
Ticker blinker;
Ticker startAP;
Ticker showTime;
Ticker syncTime;
Ticker syncRTC;
const float blink_AP = 1.0;
const float blink_STA = 0.5;
const float blink_nok = 0.1;
const char *hostname = "sntp";
const char *APclient = "sntp-config";
const char *APserver = "sntp-server";
AsyncUDP ntpServer;
RtcDS1307<TwoWire> Rtc(Wire);
const static uint32_t since2000 = 946684800U;

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
  uint32_t seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - since2000;

  std::cout << "set RTC" << std::endl;

  Rtc.SetDateTime(RtcDateTime(seconds));
}

void setTime()
{
  timeval time;
  RtcDateTime now = Rtc.GetDateTime();

  std::cout << "set time" << std::endl;

  time = {(int32_t)(now.TotalSeconds() + since2000), 0};

  settimeofday(&time, nullptr);
}

void receivedPacket(AsyncUDPPacket packet)
{
  std::cout << "receive from: " << packet.remoteIP().toString().c_str() << ":" << packet.remotePort() << ", To: " << packet.localIP().toString().c_str() << ":" << packet.localPort() << ", Length: " << packet.length() << std::endl;

  sntp.copy(packet.data());
  sntp.analyze();

  packet.write((uint8_t *)&sntp.packet, sizeof(sntp.packet));
}

void handleAP()
{
  syncTime.attach(3600, setTime);

  if (WiFi.softAP(APserver) == true)
  {
    blinker.attach(blink_AP, blink);

    if (ntpServer.listen(ntpPort))
    {
      std::cout << "UDP Listening on IP: " << WiFi.softAPIP().toString().c_str() << std::endl;
      ntpServer.onPacket(receivedPacket);
    }
  }
}

boolean handleSTA()
{
  WiFiManager wifiManager;

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  blinker.attach(blink_STA, blink);

  wifiManager.setDebugOutput(true);
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

void showClock()
{
  uint32_t timeCountSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  uint64_t timeCountMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  time_t time = timeCountSec;

  std::cout << "millis: " << millis() << " / " << (uint64_t)timeCountMillis << "; epoch (1970): " << timeCountSec << "; date: " << ctime(&time);
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
  std::cout << "\n\n"
            << std::endl;

  Rtc.Begin();

  pinMode(LED_BUILTIN, OUTPUT);

  setupRTC();

  showClock();
  setTime();
  showClock();

  blinker.attach(blink_nok, blink);
  showTime.attach(60, showClock);

  if (handleSTA())
  {
    syncRTC.once(9, setRTC);
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