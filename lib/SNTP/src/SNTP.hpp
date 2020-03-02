#ifndef SNTP_HPP
#define SNTP_HPP

// ntp server: https://www.lloydm.net/Demos/NTP/NTP-GPS-RTC_Server.ino.txt

#include <cstdint>
#include <bitset>
#include <chrono>
// #include <ctime>
#include <iostream>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#elif WIN32
#include <winsock2.h>
#elif OSX
#endif

struct tv
{
  uint32_t tv_sec;
  uint32_t tv_usec;
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

  tv referenceTimestamp; // Reference Timestamp: This field is the time the system clock was last set or corrected, in 64-bit timestamp format.
  tv originateTimestamp; // Originate Timestamp: This is the time at which the request departed the client for the server, in 64-bit timestamp format.
  tv receiveTimestamp;   // Receive Timestamp: This is the time at which the request arrived at the server or the reply arrived at the client, in 64-bit timestamp format.
  tv transmitTimestamp;  // Transmit Timestamp: This is the time at which the request departed the client or the reply departed the server, in 64-bit timestamp format.
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
  void print();
  void prepareClient();
  void analyze();
  void copy(const uint8_t *src);
  timeval getOffset();
  timeval getDelay();

private:
  const static uint32_t epochDiff = 2208988800U; // 70 years
  const static uint64_t factorFractions = 1000000000ULL; // nano seconds

  tv referenceTimestamp; // last synced
  tv T1;                 // time request sent by client
  tv T2;                 // time request received by server
  tv T3;                 // time reply sent by server
  tv T4;                 // time reply received by client
  tv now();              // based on 1900 epoch
};

#endif