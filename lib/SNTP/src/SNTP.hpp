#ifndef SNTP_HPP
#define SNTP_HPP

// ntp server: https://www.lloydm.net/Demos/NTP/NTP-GPS-RTC_Server.ino.txt

#include <cstdint>
#include <bitset>
#include <chrono>
#include <ctime>
#include <iostream>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif WIN32
#include <winsock2.h>
#elif OSX
#endif

struct TimeFormat
{
  uint32_t seconds;
  uint32_t fragments;
};

struct Packet
{

  uint8_t li_vn_mode; // Eight bits. li, vn, and mode.
                      // li.   Two bits.   Leap indicator. This field is significant only in server messages.
                      // vn.   Three bits. Version number of the protocol.
                      // mode. Three bits. Client will pick mode 3 for client.

  // This field is significant only in server messages.
  uint8_t stratum;  // Eight bits. Stratum level of the local clock.
  uint8_t poll;     // Eight bits. Maximum interval between successive messages.
  int8_t precision; // Eight bits. Precision of the local clock.

  // This field is significant only in server messages.
  int32_t rootDelay;           // 32 bits. Total round trip delay time.
  uint32_t rootDispersion;     // 32 bits. Max error aloud from primary clock source.
  char referenceIdentifier[4]; // 32 bits. Reference clock identifier.

  TimeFormat referenceTimestamp; // Reference Timestamp: This field is the time the system clock was last set or corrected, in 64-bit timestamp format.
  TimeFormat originateTimestamp; // Originate Timestamp: This is the time at which the request departed the client for the server, in 64-bit timestamp format.
  TimeFormat receiveTimestamp;   // Receive Timestamp: This is the time at which the request arrived at the server or the reply arrived at the client, in 64-bit timestamp format.
  TimeFormat transmitTimestamp;  // Transmit Timestamp: This is the time at which the request departed the client or the reply departed the server, in 64-bit timestamp format.
};

enum LI
{
  noWarning = 0,
  lastMinute61 = 1,
  lastMinute59 = 2,
  alarmCondition = 3,
};

enum MODE
{
  reserved = 0,
  symmetricActive = 1,
  symmetricPassive = 2,
  client = 3,
  server = 4,
  broadcast = 5,
  controlMessage = 6,
  privateUse = 7
};

enum VERSION
{
  v3 = 3,
  v4 = 4
};

class SNTP
{
public:
  SNTP();
  ~SNTP();
  Packet packet;
  uint32_t t; // offset
  uint32_t d; // delay
  void printPacket();
  void printDate(uint32_t seconds);
  void prepareClient();
  void analyze();
  void copy(const uint8_t* src);

private:
  const static uint32_t epochDiff = 2208988800ull; // 70 years

  TimeFormat referenceTimestamp; // last synced
  TimeFormat T1;                 // time request sent by client
  TimeFormat T2;                 // time request received by server
  TimeFormat T3;                 // time reply sent by server
  TimeFormat T4;                 // time reply received by client
  TimeFormat now();                // based on 1900 epoch
};

#endif