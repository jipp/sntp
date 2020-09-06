#include <Arduino.h>

// TODO(jipp): check if ntp is synced

#include <iostream>

#include <ESPAsyncUDP.h>
#include <RtcDS1307.h>
#include <Ticker.h>
#include <Wire.h>

#include "SNTP.hpp"

#ifndef SPEED
#define SPEED 115200
#endif

const char *hostname = "sntp";
const char *APNameConfig = "sntp-config";
const char *APNameServer = "sntp-server";
const int syncTimeInterval = 36000;
const int showTimeInterval = 60;
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
  digitalWrite(LED_BUILTIN, static_cast<uint8_t>(!(digitalRead(LED_BUILTIN) == 1)));
}

void receivedPacket(AsyncUDPPacket packet)
{
  std::cout << "receive from: " << packet.remoteIP().toString().c_str() << ":" << packet.remotePort() << ", To: " << packet.localIP().toString().c_str() << ":" << packet.localPort() << ", Length: " << packet.length() << std::endl;

  sntp.copy(packet.data());
  sntp.analyze();

  packet.write(reinterpret_cast<uint8_t *>(&sntp.packet), sizeof(sntp.packet));
}

void showTime()
{
  uint32_t timeCountSec = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
  uint64_t timeCountMillis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  uint64_t countMillis = millis();
  time_t time = timeCountSec;

  std::cout << "millis: " << countMillis << " / " << timeCountMillis << "; epoch (1970): " << timeCountSec << "; date: " << ctime(&time);
}

void setupRTC()
{
  Rtc.Begin();

  if (!Rtc.IsDateTimeValid())
  {
    if (Rtc.LastError() != 0)
    {
      std::cout << "RTC communications error = " << static_cast<uint16_t>(Rtc.LastError()) << std::endl;
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
  RtcDateTime now = Rtc.GetDateTime();
  timeval time = {static_cast<int32_t>((now.TotalSeconds() + since2000)), 0};

  showTime();
  settimeofday(&time, nullptr);
  showTime();
}

// void setRTC()
// {
//   uint32_t seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - since2000;

//   Rtc.SetDateTime(RtcDateTime(seconds));
// }

bool startSNTP()
{
  bool status = ntpServer.listen(ntpPort);

  if (status)
  {
    std::cout << "UDP Listening on IP:port: " << WiFi.softAPIP().toString().c_str() << ":" << ntpPort << std::endl;
    ntpServer.onPacket(receivedPacket);
  }

  return status;
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
  shower.attach(showTimeInterval, showTime);

  if (startAP())
  {
    std::cout << "AP started" << std::endl;
    syncer.attach(syncTimeInterval, syncTime);
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