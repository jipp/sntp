#ifndef SNTP_HPP
#define SNTP_HPP

#include <cstdint>
#include <bitset>
#include <chrono>
#include <ctime> 
#include <iostream>

struct NTP_PACKET
{

  uint8_t li_vn_mode; // Eight bits. li, vn, and mode.
                      // li.   Two bits.   Leap indicator.
                      // vn.   Three bits. Version number of the protocol.
                      // mode. Three bits. Client will pick mode 3 for client.

  uint8_t stratum;  // Eight bits. Stratum level of the local clock.
  uint8_t poll;     // Eight bits. Maximum interval between successive messages.
  int8_t precision; // Eight bits. Precision of the local clock.

  int32_t rootDelay;            // 32 bits. Total round trip delay time.
  uint32_t rootDispersion;      // 32 bits. Max error aloud from primary clock source.
  uint32_t ReferenceIdentifier; // 32 bits. Reference clock identifier.

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

  uint32_t keyIdentifier;    // 32 bits optional
  uint8_t messageDigest[16]; // 128 bits optional
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
  void printPacket();
  void printDate();
  void clientPacketPrepare();
  void serverPacketPrepare();

private:
  NTP_PACKET ntpPacket;
  uint32_t offset_s = 0;
  uint32_t offset_f = 0;
  uint32_t referenceTimestamp_s = 0;
  uint32_t referenceTimestamp_f = 0;
};

#endif