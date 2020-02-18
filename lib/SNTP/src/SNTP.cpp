#include "SNTP.hpp"

SNTP::SNTP()
{
    ntpPacket = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

SNTP::~SNTP(void) = default;

void SNTP::printPacket()
{
    std::cout << std::endl;
    std::cout << "li_vn_mode: 0b" << std::bitset<8>(ntpPacket.li_vn_mode) << std::endl;
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
    std::cout << std::endl;
}

void SNTP::printDate()
{
    uint32_t delta = referenceTimestamp_s + offset_s;
    time_t time = delta;

    std::cout << delta << " " << ctime(&time) << std::endl;
}

void SNTP::clientPacketPrepare()
{
    ntpPacket.li_vn_mode = (LI::noWarning << 6) + (VERSION::v3 << 3) + MODE::client;

    referenceTimestamp_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    referenceTimestamp_f = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - referenceTimestamp_s * 1000;

    ntpPacket.referenceTimestamp_s = referenceTimestamp_s;
    ntpPacket.referenceTimestamp_f = referenceTimestamp_f;
}

void SNTP::serverPacketPrepare()
{
    offset_s = 1804800000;
    offset_f = 0;
}