#include <Arduino.h>

// TODO(jipp): check if ntp is synced
// TODO(jipp): add webinterface to enter time

#include <iostream>

#include <ESPAsyncUDP.h>
#include <ESPAsyncWebServer.h>
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
AsyncWebServer webServer(80);
const char *PARAM_INPUT = "epochTime";

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Epoch Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    epoch time: <input type="number" name="epochTime">
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

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

void syncTimeFromRTC()
{
  RtcDateTime now = Rtc.GetDateTime();
  timeval time = {static_cast<int32_t>((now.TotalSeconds() + since2000)), 0};

  showTime();
  settimeofday(&time, nullptr);
  showTime();
}

void setRTC()
{
  uint32_t seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - since2000;

  Rtc.SetDateTime(RtcDateTime(seconds));
}

void setRTC(uint32_t epoch)
{
  uint32_t seconds = epoch - since2000;

  Rtc.SetDateTime(RtcDateTime(seconds));
}

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

void startWebServer()
{
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  webServer.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    uint32_t epoch = 0;

    if (request->hasParam(PARAM_INPUT))
    {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      inputParam = PARAM_INPUT;
    }
    else
    {
      inputMessage = "No message sent";
      inputParam = "none";
    }

    std::cout << "input epoch (String): " << inputMessage.c_str() << std::endl;
    epoch = strtoul(inputMessage.c_str(), nullptr, 10);
    std::cout << "input epoch (uint32_t): " << epoch << std::endl;

    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" + inputParam + ") with value: " + inputMessage + "<br><a href=\"/\">Return to Home Page</a>");

    setRTC(epoch);
    syncTimeFromRTC();
  });

  webServer.onNotFound(notFound);

  webServer.begin();

  std::cout << "Web Server started" << std::endl;
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
  syncTimeFromRTC();
  shower.attach(showTimeInterval, showTime);

  if (startAP())
  {
    std::cout << "AP started" << std::endl;
    syncer.attach(syncTimeInterval, syncTimeFromRTC);
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

  startWebServer();
}

void loop()
{
}