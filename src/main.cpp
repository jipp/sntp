#include <Arduino.h>

// TODO(jipp): check if ntp is synced

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

const char *hostname = "sntp";
const char *APNameConfig = "sntp-config";
const char *APNameServer = "sntp-server";
Ticker blinker;
Ticker shower;
Ticker syncer;
const float blink_ok = 1.0;
const float blink_wait = 0.5;
const float blink_nok = 0.1;
SNTP sntp = SNTP();
const uint16_t ntpPort = 123;
AsyncUDP ntpServer;
RtcDS1307<TwoWire> Rtc(Wire);
const static uint32_t since2000 = 946684800U;

void blink()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void configModeCallback(WiFiManager *wifiManager)
{
  blinker.attach(blink_wait, blink);
}

void receivedPacket(AsyncUDPPacket packet)
{
  std::cout << "receive from: " << packet.remoteIP().toString().c_str() << ":" << packet.remotePort() << ", To: " << packet.localIP().toString().c_str() << ":" << packet.localPort() << ", Length: " << packet.length() << std::endl;

  sntp.copy(packet.data());
  sntp.analyze();

  packet.write((uint8_t *)&sntp.packet, sizeof(sntp.packet));
}

void showTime()
{
  uint32_t timeCountSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  uint64_t timeCountMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  uint64_t countMillis = millis();
  time_t time = timeCountSec;

  std::cout << "millis: " << countMillis << " / " << (uint64_t)timeCountMillis << "; epoch (1970): " << timeCountSec << "; date: " << ctime(&time);
}

void setupRTC()
{
  Rtc.Begin();

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

bool startAP()
{
  WiFi.mode(WIFI_AP);

  return WiFi.softAP(APNameServer);
}

void syncTime()
{
  timeval time;
  RtcDateTime now = Rtc.GetDateTime();

  time = {(int32_t)(now.TotalSeconds() + since2000), 0};

  showTime();
  settimeofday(&time, nullptr);
  showTime();
}

void setRTC()
{
  uint32_t seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - since2000;

  Rtc.SetDateTime(RtcDateTime(seconds));
}

bool startSNTP()
{
  if (ntpServer.listen(ntpPort))
  {
    std::cout << "UDP Listening on IP:port: " << WiFi.softAPIP().toString().c_str() << ":" << ntpPort << std::endl;
    ntpServer.onPacket(receivedPacket);

    return true;
  }

  return false;
}

void setup()
{
  Serial.begin(SPEED);
  std::cout << "\n\n"
            << std::endl;

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  blinker.attach(blink_wait, blink);

  setupRTC();
  syncTime();
  shower.attach(60, showTime);

  if (startAP())
  {
    std::cout << "AP started" << std::endl;
    syncer.attach(36000, syncTime);
    if (startSNTP())
    {
      blinker.attach(blink_ok, blink);
    }
    else
    {
      std::cout << "sntpd not started" << std::endl;
      blinker.attach(blink_nok, blink);
    };
  }
  else
  {
    {
      std::cout << "AP not started" << std::endl;
    }
  }
}

void loop()
{
}