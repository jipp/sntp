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

    referenceTimestamp.seconds = 0;
    referenceTimestamp.fragments = 0;
    ntpPacket.referenceTimestamp.seconds = 0;
    ntpPacket.referenceTimestamp.fragments = 0;

    ntpPacket.originateTimestamp.seconds = 0;
    ntpPacket.originateTimestamp.fragments = 0;

    ntpPacket.receiveTimestamp.seconds = 0;
    ntpPacket.receiveTimestamp.fragments = 0;

    ntpPacket.transmitTimestamp.seconds = 0;
    ntpPacket.transmitTimestamp.fragments = 0;
}

SNTP::~SNTP(void) = default;

void SNTP::printPacket()
{
    std::cout << std::endl;
    std::cout << "li_vn_mode: 0b" << std::bitset<8>(ntpPacket.li_vn_mode) << std::endl;
    std::cout << "stratum: " << (uint16_t)ntpPacket.stratum << std::endl;
    std::cout << "poll: " << (uint16_t)ntpPacket.poll << std::endl;
    std::cout << "precision: " << (uint16_t)ntpPacket.precision << std::endl;
    std::cout << "rootDelay: " << ntohl(ntpPacket.rootDelay) << std::endl;
    std::cout << "rootDispersion: " << ntohl(ntpPacket.rootDispersion) << std::endl;
    std::cout << "ReferenceIdentifier: " << ntpPacket.referenceIdentifier[0] << ntpPacket.referenceIdentifier[1] << ntpPacket.referenceIdentifier[2] << ntpPacket.referenceIdentifier[3] << std::endl;

    std::cout << "referenceTimestamp_s: " << ntohl(ntpPacket.referenceTimestamp.seconds) << std::endl;
    std::cout << "referenceTimestamp_f: " << ntohl(ntpPacket.referenceTimestamp.fragments) << std::endl;
    std::cout << "originateTimestamp_s: " << ntohl(ntpPacket.originateTimestamp.seconds) << std::endl;
    std::cout << "originateTimestamp_f: " << ntohl(ntpPacket.originateTimestamp.fragments) << std::endl;
    std::cout << "T2_s / receiveTimestamp_s: " << ntohl(ntpPacket.receiveTimestamp.seconds) << std::endl;
    std::cout << "T2_f / receiveTimestamp_f: " << ntohl(ntpPacket.receiveTimestamp.fragments) << std::endl;
    std::cout << "T3_s / transmitTimestamp_s: " << ntohl(ntpPacket.transmitTimestamp.seconds) << std::endl;
    std::cout << "T3_f / transmitTimestamp_f: " << ntohl(ntpPacket.transmitTimestamp.fragments) << std::endl;

    std::cout << "T1_s: " << T1.seconds << std::endl;
    std::cout << "T1_f: " << T1.fragments << std::endl;

    std::cout << "T4_s: " << T1.seconds << std::endl;
    std::cout << "T4_f: " << T1.fragments << std::endl;

    std::cout << std::endl;
}

void SNTP::printDate()
{
    time_t time = T1.seconds - epochDiff;

    std::cout << ctime(&time) << std::endl;
}

void SNTP::clientPacketPrepare()
{
    uint64_t timeStamp;

    timeStamp = now();
    T1.seconds = timeStamp / 1000;
    T1.fragments = timeStamp - T1.seconds * 1000;

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

    ntpPacket.referenceTimestamp.seconds = htonl(referenceTimestamp.seconds);
    ntpPacket.referenceTimestamp.fragments = htonl(referenceTimestamp.fragments);

    ntpPacket.originateTimestamp.seconds = htonl(T1.seconds);
    ntpPacket.originateTimestamp.fragments = htonl(T1.fragments);

    ntpPacket.receiveTimestamp.seconds = 0;
    ntpPacket.receiveTimestamp.fragments = 0;

    ntpPacket.transmitTimestamp.seconds = 0;
    ntpPacket.transmitTimestamp.fragments = 0;
}

void SNTP::packetAnalyze()
{
    uint64_t timeStamp;

    switch (ntpPacket.li_vn_mode & 0b00000111)
    {
    case MODE::server:
        timeStamp = now();
        T2.seconds = ntohl(ntpPacket.receiveTimestamp.seconds);
        T2.fragments = ntohl(ntpPacket.receiveTimestamp.fragments);
        T3.seconds = ntohl(ntpPacket.transmitTimestamp.seconds);
        T3.fragments = ntohl(ntpPacket.transmitTimestamp.fragments);
        T4.seconds = timeStamp / 1000;
        T4.fragments = timeStamp - T4.seconds * 1000;

        t = ((T2.seconds - T1.seconds) + (T3.seconds - T4.seconds)) / 2;
        d = (T4.seconds - T1.seconds) - (T3.seconds - T2.seconds);

        timeStamp = now();
        referenceTimestamp.seconds = timeStamp / 1000;
        referenceTimestamp.fragments = timeStamp - referenceTimestamp.seconds * 1000;

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

        ntpPacket.referenceTimestamp.seconds = 0;
        ntpPacket.referenceTimestamp.fragments = 0;

        ntpPacket.originateTimestamp.seconds = ntohl(T1.seconds);
        ntpPacket.originateTimestamp.fragments = ntohl(T1.fragments);

        ntpPacket.receiveTimestamp.seconds = 0;
        ntpPacket.receiveTimestamp.fragments = 0;

        ntpPacket.transmitTimestamp.seconds = 0;
        ntpPacket.transmitTimestamp.fragments = 0;

        break;
    }
}

uint64_t SNTP::now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - epochDiff * 1000;
}