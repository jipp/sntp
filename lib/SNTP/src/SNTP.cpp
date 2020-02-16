#include "SNTP.hpp"

SNTP::SNTP()
{
    ntpPacket = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

SNTP::~SNTP(void) = default;

void SNTP::printPacket()
{
    std::cout << "li_vn_mode: 0b" << std::bitset<8> (ntpPacket.li_vn_mode) << std::endl;
    std::cout << "stratum: " << (uint16_t)ntpPacket.stratum << std::endl;
    std::cout << "poll: " << (uint16_t)ntpPacket.poll << std::endl;
    std::cout << "precision: " << (uint16_t)ntpPacket.precision << std::endl;
    std::cout << "rootDelay: " << ntpPacket.rootDelay << std::endl;
    std::cout << "rootDispersion: " << ntpPacket.rootDispersion << std::endl;
    std::cout << "ReferenceIdentifier: " << ntpPacket.ReferenceIdentifier << std::endl;
    std::cout << "referenceTimestamp_s: " << ntpPacket.referenceTimestamp_s << std::endl;
    std::cout << "referenceTimestamp_f: " << ntpPacket.referenceTimestamp_f << std::endl;
    std::cout << "originateTimestamp_s: " << ntpPacket.originateTimestamp_s << std::endl;
    std::cout << "originateTimestamp_f: " << ntpPacket.originateTimestamp_f << std::endl;
    std::cout << "receiveTimestamp_s: " << ntpPacket.receiveTimestamp_s << std::endl;
    std::cout << "receiveTimestamp_f: " << ntpPacket.receiveTimestamp_f << std::endl;
    std::cout << "transmitTimestamp_s: " << ntpPacket.transmitTimestamp_s << std::endl;
    std::cout << "transmitTimestamp_f: " << ntpPacket.transmitTimestamp_f << std::endl;
}

void SNTP::clientPacket()
{
    ntpPacket.li_vn_mode = (LI::noWarning << 6) + (VERSION::v3 << 3) + MODE::client;
}