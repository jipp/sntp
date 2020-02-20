#include "SNTP.hpp"

SNTP::SNTP()
{
    ntpPacket.li_vn_mode = 0;

    ntpPacket.stratum = 0;
    ntpPacket.poll = 0;
    ntpPacket.precision = 0;

    ntpPacket.rootDelay = 0;
    ntpPacket.rootDispersion = 0;
    ntpPacket.referenceIdentifier[0] = ' ';
    ntpPacket.referenceIdentifier[1] = ' ';
    ntpPacket.referenceIdentifier[2] = ' ';
    ntpPacket.referenceIdentifier[3] = ' ';

    ntpPacket.referenceTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
    ntpPacket.referenceTimestamp_f = 0;

    ntpPacket.originateTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
    ntpPacket.originateTimestamp_f = 0;

    ntpPacket.receiveTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
    ntpPacket.receiveTimestamp_f = 0;

    ntpPacket.transmitTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
    ntpPacket.transmitTimestamp_f = 0;
}

SNTP::~SNTP(void) = default;

void SNTP::printPacket()
{
    std::cout << std::endl;
    std::cout << "li_vn_mode: 0b" << std::bitset<8>(ntpPacket.li_vn_mode) << std::endl;
    std::cout << "stratum: " << (uint16_t)ntpPacket.stratum << std::endl;
    std::cout << "poll: " << (uint16_t)ntpPacket.poll << std::endl;
    std::cout << "precision: " << (uint16_t)ntpPacket.precision << std::endl;
    std::cout << "rootDelay: " << htonl(ntpPacket.rootDelay) << std::endl;
    std::cout << "rootDispersion: " << htonl(ntpPacket.rootDispersion) << std::endl;
    std::cout << "ReferenceIdentifier: " << ntpPacket.referenceIdentifier << std::endl
              << std::endl;

    std::cout << "referenceTimestamp_s: " << htonl(ntpPacket.referenceTimestamp_s) - NTP_TIMESTAMP_DELTA << std::endl;
    std::cout << "referenceTimestamp_f: " << htonl(ntpPacket.referenceTimestamp_f) << std::endl;
    std::cout << "originateTimestamp_s: " << htonl(ntpPacket.originateTimestamp_s) - NTP_TIMESTAMP_DELTA << std::endl;
    std::cout << "originateTimestamp_f: " << htonl(ntpPacket.originateTimestamp_f) << std::endl;
    std::cout << "T2_s / receiveTimestamp_s: " << htonl(ntpPacket.receiveTimestamp_s) - NTP_TIMESTAMP_DELTA << std::endl;
    std::cout << "T2_f / receiveTimestamp_f: " << htonl(ntpPacket.receiveTimestamp_f) << std::endl;
    std::cout << "T3_s / transmitTimestamp_s: " << htonl(ntpPacket.transmitTimestamp_s) - NTP_TIMESTAMP_DELTA << std::endl;
    std::cout << "T3_f / transmitTimestamp_f: " << htonl(ntpPacket.transmitTimestamp_f) << std::endl;

    std::cout << "T1_s: " << T1_s << std::endl;
    std::cout << "T1_f: " << T1_f << std::endl;

    std::cout << "T4_s: " << T1_s << std::endl;
    std::cout << "T4_f: " << T1_f << std::endl;

    std::cout << std::endl;
}

void SNTP::printDate()
{
    time_t time = T1_s;

    std::cout << ctime(&time) << std::endl;
}

void SNTP::clientPacketPrepare()
{
    T1_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    T1_f = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - T1_s * 1000;

    ntpPacket.li_vn_mode = (LI::noWarning << 6) | (VERSION::v3 << 3) | MODE::client;

    ntpPacket.stratum = 0;
    ntpPacket.poll = 0;
    ntpPacket.precision = 0;

    ntpPacket.rootDelay = 0;
    ntpPacket.rootDispersion = 0;
    ntpPacket.referenceIdentifier[0] = ' ';
    ntpPacket.referenceIdentifier[1] = ' ';
    ntpPacket.referenceIdentifier[2] = ' ';
    ntpPacket.referenceIdentifier[3] = ' ';

    ntpPacket.referenceTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
    ntpPacket.referenceTimestamp_f = 0;

    ntpPacket.originateTimestamp_s = htonl(T1_s + NTP_TIMESTAMP_DELTA);
    ntpPacket.originateTimestamp_f = htonl(T1_f);

    ntpPacket.receiveTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
    ntpPacket.receiveTimestamp_f = 0;

    ntpPacket.transmitTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
    ntpPacket.transmitTimestamp_f = 0;
}

void SNTP::packetAnalyze()
{
    switch (ntpPacket.li_vn_mode & 0b00000111)
    {
    case MODE::server:
        T2_s = htonl(ntpPacket.receiveTimestamp_s) - NTP_TIMESTAMP_DELTA;
        T2_f = htonl(ntpPacket.receiveTimestamp_f);
        T3_s = htonl(ntpPacket.transmitTimestamp_s) - NTP_TIMESTAMP_DELTA;
        T3_f = htonl(ntpPacket.transmitTimestamp_f);
        T4_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        T4_f = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - T1_s * 1000;
        t = ((T2_s - T1_s) + (T3_s - T4_s)) / 2;
        d = (T4_s - T1_s) - (T3_s - T2_s);
        break;
    case MODE::client:
        ntpPacket.li_vn_mode = (LI::noWarning << 6) | (VERSION::v3 << 3) | MODE::server;

        ntpPacket.stratum = 2;
        ntpPacket.poll = 6;
        ntpPacket.precision = 0xF7;

        ntpPacket.rootDelay = 0x0080;
        ntpPacket.rootDispersion = 0x00c0;
        ntpPacket.referenceIdentifier[0] = 'L';
        ntpPacket.referenceIdentifier[1] = 'O';
        ntpPacket.referenceIdentifier[2] = 'C';
        ntpPacket.referenceIdentifier[3] = 'L';

        ntpPacket.referenceTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
        ntpPacket.referenceTimestamp_f = 0;

        ntpPacket.originateTimestamp_s = htonl(T1_s + NTP_TIMESTAMP_DELTA);
        ntpPacket.originateTimestamp_f = htonl(T1_f);

        ntpPacket.receiveTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
        ntpPacket.receiveTimestamp_f = 0;

        ntpPacket.transmitTimestamp_s = htonl(0 + NTP_TIMESTAMP_DELTA);
        ntpPacket.transmitTimestamp_f = 0;

        break;
    }
}