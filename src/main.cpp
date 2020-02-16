#include <Arduino.h>

#include "SNTP.hpp"

SNTP sntp = SNTP();

void setup()
{
  Serial.begin(115200);
  sntp.printPacket();
  sntp.clientPacket();
  sntp.printPacket();
}

void loop()
{
  // put your main code here, to run repeatedly:
}