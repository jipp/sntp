#include "SNTP.hpp"

SNTP::SNTP()
{
    packet.li_vn_mode = 0;

    packet.stratum = 0;
    packet.poll = 0;
    packet.precision = 0;

    packet.rootDelay = 0;
    packet.rootDispersion = 0;
    packet.referenceIdentifier[0] = ' ';
    packet.referenceIdentifier[1] = ' ';
    packet.referenceIdentifier[2] = ' ';
    packet.referenceIdentifier[3] = ' ';

    referenceTimestamp.seconds = 0;
    referenceTimestamp.fragments = 0;
    packet.referenceTimestamp.seconds = 0;
    packet.referenceTimestamp.fragments = 0;

    packet.originateTimestamp.seconds = 0;
    packet.originateTimestamp.fragments = 0;

    packet.receiveTimestamp.seconds = 0;
    packet.receiveTimestamp.fragments = 0;

    packet.transmitTimestamp.seconds = 0;
    packet.transmitTimestamp.fragments = 0;
}

SNTP::~SNTP(void) = default;

void SNTP::printPacket()
{
    std::cout << std::endl;
    std::cout << "li_vn_mode: 0b" << std::bitset<8>(packet.li_vn_mode) << std::endl;
    std::cout << "stratum: " << (uint16_t)packet.stratum << std::endl;
    std::cout << "poll: " << (uint16_t)packet.poll << std::endl;
    std::cout << "precision: " << (uint16_t)packet.precision << std::endl;
    std::cout << "rootDelay: " << ntohl(packet.rootDelay) << std::endl;
    std::cout << "rootDispersion: " << ntohl(packet.rootDispersion) << std::endl;
    std::cout << "ReferenceIdentifier: " << packet.referenceIdentifier[0] << packet.referenceIdentifier[1] << packet.referenceIdentifier[2] << packet.referenceIdentifier[3] << std::endl;

    std::cout << "referenceTimestamp_s: " << ntohl(packet.referenceTimestamp.seconds) << std::endl;
    std::cout << "referenceTimestamp_f: " << ntohl(packet.referenceTimestamp.fragments) << std::endl;
    std::cout << "originateTimestamp_s: " << ntohl(packet.originateTimestamp.seconds) << std::endl;
    std::cout << "originateTimestamp_f: " << ntohl(packet.originateTimestamp.fragments) << std::endl;
    std::cout << "T1_s: " << T1.seconds << std::endl;
    std::cout << "T1_f: " << T1.fragments << std::endl;

    std::cout << "T2_s / receiveTimestamp_s: " << ntohl(packet.receiveTimestamp.seconds) << std::endl;
    std::cout << "T2_f / receiveTimestamp_f: " << ntohl(packet.receiveTimestamp.fragments) << std::endl;

    std::cout << "T3_s / transmitTimestamp_s: " << ntohl(packet.transmitTimestamp.seconds) << std::endl;
    std::cout << "T3_f / transmitTimestamp_f: " << ntohl(packet.transmitTimestamp.fragments) << std::endl;

    std::cout << "T4_s: " << T4.seconds << std::endl;
    std::cout << "T4_f: " << T4.fragments << std::endl;

    std::cout << std::endl;
}

void SNTP::printDate(uint32_t seconds)
{
    time_t time = seconds - epochDiff;

    std::cout << ctime(&time) << std::endl;
}

void SNTP::prepareClient()
{
    TimeFormat timeStamp;

    timeStamp = now();
    T1.seconds = timeStamp.seconds;
    T1.fragments = timeStamp.fragments;

    packet.li_vn_mode = (LI::noWarning << 6) | (VERSION::v3 << 3) | MODE::client;

    packet.stratum = 0;
    packet.poll = 0;
    packet.precision = 0;

    packet.rootDelay = 0;
    packet.rootDispersion = 0;
    packet.referenceIdentifier[0] = ' ';
    packet.referenceIdentifier[1] = ' ';
    packet.referenceIdentifier[2] = ' ';
    packet.referenceIdentifier[3] = ' ';

    packet.referenceTimestamp.seconds = htonl(referenceTimestamp.seconds);
    packet.referenceTimestamp.fragments = htonl(referenceTimestamp.fragments);

    packet.originateTimestamp.seconds = htonl(T1.seconds);
    packet.originateTimestamp.fragments = htonl(T1.fragments);

    packet.receiveTimestamp.seconds = 0;
    packet.receiveTimestamp.fragments = 0;

    packet.transmitTimestamp.seconds = 0;
    packet.transmitTimestamp.fragments = 0;
}

void SNTP::analyze()
{
    TimeFormat timeStamp;

    switch (packet.li_vn_mode & 0b00000111)
    {
    case MODE::server:
        //std::cout << "server" << std::cout;
        timeStamp = now();

        T2.seconds = ntohl(packet.receiveTimestamp.seconds);
        T2.fragments = ntohl(packet.receiveTimestamp.fragments);
        T3.seconds = ntohl(packet.transmitTimestamp.seconds);
        T3.fragments = ntohl(packet.transmitTimestamp.fragments);
        T4.seconds = timeStamp.seconds;
        T4.fragments = timeStamp.fragments;

        t = ((T2.seconds - T1.seconds) + (T3.seconds - T4.seconds)) / 2;
        d = (T4.seconds - T1.seconds) - (T3.seconds - T2.seconds);
        timeStamp = now();

        referenceTimestamp.seconds = timeStamp.seconds;
        referenceTimestamp.fragments = timeStamp.fragments;

        break;

    case MODE::client:
        std::cout << "client" << std::cout;
        timeStamp = now();

        packet.li_vn_mode = (LI::noWarning << 6) | (VERSION::v3 << 3) | MODE::server;

        packet.stratum = 2;
        packet.poll = 6;
        packet.precision = 0xF7;

        packet.rootDelay = 0x0080;
        packet.rootDispersion = 0x00c0;
        packet.referenceIdentifier[0] = 'L';
        packet.referenceIdentifier[1] = 'O';
        packet.referenceIdentifier[2] = 'C';
        packet.referenceIdentifier[3] = 'L';

        packet.referenceTimestamp.seconds = htonl(referenceTimestamp.seconds);
        packet.referenceTimestamp.fragments = htonl(referenceTimestamp.fragments);

        packet.originateTimestamp.seconds = htonl(T1.seconds);
        packet.originateTimestamp.fragments = htonl(T1.fragments);

        packet.receiveTimestamp.seconds = htonl(timeStamp.seconds);
        packet.receiveTimestamp.fragments = htonl(timeStamp.fragments);

        timeStamp = now();
        packet.transmitTimestamp.seconds = htonl(timeStamp.seconds);
        packet.transmitTimestamp.fragments = htonl(timeStamp.fragments);

        break;
    }
}

TimeFormat SNTP::now()
{
    TimeFormat timeStamp;
    static const uint64_t factor = 1000000;
    uint64_t count = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + epochDiff * factor;

    timeStamp.seconds = count / factor;
    timeStamp.fragments = count - timeStamp.seconds * factor;

    return timeStamp;
}

void SNTP::copy(const uint8_t *src)
{
    memcpy((char *)&packet, src, sizeof(packet));
}