#include <Arduino.h>

#include "SNTP.hpp"

#ifndef SPEED
#define SPEED 115200
#endif

SNTP sntp = SNTP();

void setup()
{
  Serial.begin(SPEED);

  delay(1000);
  sntp.clientPacketPrepare();

  sntp.printDate();

  delay(5000);
  sntp.clientPacketPrepare();

  sntp.printDate();  
  
  delay(5000);
  sntp.clientPacketPrepare();

  sntp.printDate();
}

void loop()
{
}