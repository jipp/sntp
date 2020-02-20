#ifndef SNTP_HPP
#define SNTP_HPP

// ntp server: https://www.lloydm.net/Demos/NTP/NTP-GPS-RTC_Server.ino.txt

#include <cstdint>
#include <bitset>
#include <chrono>
#include <ctime>
#include <iostream>
#ifdef ESP8266
#include <ethernet.h>
#elif WIN32
#include <winsock2.h>
#endif

struct NTP_PACKET
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
  int32_t rootDelay;            // 32 bits. Total round trip delay time.
  uint32_t rootDispersion;      // 32 bits. Max error aloud from primary clock source.
  char referenceIdentifier[4]; // 32 bits. Reference clock identifier.

  // Reference Timestamp: This field is the time the system clock was last set or corrected, in 64-bit timestamp format.
  uint32_t referenceTimestamp_s; // 32 bits. Reference time-stamp seconds.
  uint32_t referenceTimestamp_f; // 32 bits. Reference time-stamp fraction of a second.

  // Originate Timestamp: This is the time at which the request departed the client for the server, in 64-bit timestamp format.
  uint32_t originateTimestamp_s; // 32 bits. Originate time-stamp seconds.
  uint32_t originateTimestamp_f; // 32 bits. Originate time-stamp fraction of a second.

  // Receive Timestamp: This is the time at which the request arrived at the server or the reply arrived at the client, in 64-bit timestamp format.
  uint32_t receiveTimestamp_s; // 32 bits. Received time-stamp seconds.
  uint32_t receiveTimestamp_f; // 32 bits. Received time-stamp fraction of a second.

  // Transmit Timestamp: This is the time at which the request departed the client or the reply departed the server, in 64-bit timestamp format.
  uint32_t transmitTimestamp_s; // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t transmitTimestamp_f; // 32 bits. Transmit time-stamp fraction of a second.
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
  NTP_PACKET ntpPacket;
  uint32_t t;
  uint32_t d;
  void printPacket();
  void printDate();
  void clientPacketPrepare();
  void packetAnalyze();

private:
  const uint32_t NTP_TIMESTAMP_DELTA = 2208988800ull; // 70 years
  uint32_t T1_s = 0;
  uint32_t T2_s = 0;
  uint32_t T3_s = 0;
  uint32_t T4_s = 0;
  uint32_t T1_f = 0;
  uint32_t T2_f = 0;
  uint32_t T3_f = 0;
  uint32_t T4_f = 0;
};

#endif