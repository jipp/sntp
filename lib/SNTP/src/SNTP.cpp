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

    referenceTimestamp = {0, 0};

    packet.referenceTimestamp = {0, 0};
    packet.originateTimestamp = {0, 0};
    packet.receiveTimestamp = {0, 0};
    packet.transmitTimestamp = {0, 0};
}

SNTP::~SNTP(void) = default;

void SNTP::print()
{
    std::cout << std::endl;
    std::cout << "li_vn_mode: 0b" << std::bitset<8>(packet.li_vn_mode) << std::endl;
    std::cout << "stratum: " << (uint16_t)packet.stratum << std::endl;
    std::cout << "poll: " << (uint16_t)packet.poll << std::endl;
    std::cout << "precision: " << (uint16_t)packet.precision << std::endl;
    std::cout << "rootDelay: " << ntohl(packet.rootDelay) << std::endl;
    std::cout << "rootDispersion: " << ntohl(packet.rootDispersion) << std::endl;
    std::cout << "ReferenceIdentifier: " << packet.referenceIdentifier[0] << packet.referenceIdentifier[1] << packet.referenceIdentifier[2] << packet.referenceIdentifier[3] << std::endl;

    std::cout << "referenceTimestamp_s: " << ntohl(packet.referenceTimestamp.tv_sec) << std::endl;
    std::cout << "referenceTimestamp_f: " << ntohl(packet.referenceTimestamp.tv_usec) << std::endl;
    std::cout << "originateTimestamp_s: " << ntohl(packet.originateTimestamp.tv_sec) << std::endl;
    std::cout << "originateTimestamp_f: " << ntohl(packet.originateTimestamp.tv_usec) << std::endl;
    std::cout << "T1_s: " << T1.tv_sec << std::endl;
    std::cout << "T1_f: " << T1.tv_usec << std::endl;

    std::cout << "T2_s / receiveTimestamp_s: " << ntohl(packet.receiveTimestamp.tv_sec) << std::endl;
    std::cout << "T2_f / receiveTimestamp_f: " << ntohl(packet.receiveTimestamp.tv_usec) << std::endl;

    std::cout << "T3_s / transmitTimestamp_s: " << ntohl(packet.transmitTimestamp.tv_sec) << std::endl;
    std::cout << "T3_f / transmitTimestamp_f: " << ntohl(packet.transmitTimestamp.tv_usec) << std::endl;

    std::cout << "T4_s: " << T4.tv_sec << std::endl;
    std::cout << "T4_f: " << T4.tv_usec << std::endl;

    std::cout << std::endl;
}

void SNTP::prepareClient()
{
    T1 = now();

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

    packet.referenceTimestamp.tv_sec = htonl(referenceTimestamp.tv_sec);
    packet.referenceTimestamp.tv_usec = htonl(referenceTimestamp.tv_usec);

    packet.originateTimestamp.tv_sec = htonl(T1.tv_sec);
    packet.originateTimestamp.tv_usec = htonl(T1.tv_usec);

    packet.receiveTimestamp = {0, 0};
    packet.transmitTimestamp = {0, 0};
}

timeval SNTP::getOffset()
{
    int64_t t;
    int32_t sec;
    int32_t usec;
    uint64_t T1 = this->T1.tv_sec * factor + this->T1.tv_usec;
    uint64_t T2 = this->T2.tv_sec * factor + this->T2.tv_usec;
    uint64_t T3 = this->T3.tv_sec * factor + this->T3.tv_usec;
    uint64_t T4 = this->T4.tv_sec * factor + this->T4.tv_usec;

    t = ((T2 - T1) + (T3 - T4)) / 2;

    sec = t / factor;
    usec = t - sec * factor;

    return {sec, usec};
}

timeval SNTP::getDelay()
{
    int64_t d;
    int32_t sec;
    int32_t usec;
    uint64_t T1 = this->T1.tv_sec * factor + this->T1.tv_usec;
    uint64_t T2 = this->T2.tv_sec * factor + this->T2.tv_usec;
    uint64_t T3 = this->T3.tv_sec * factor + this->T3.tv_usec;
    uint64_t T4 = this->T4.tv_sec * factor + this->T4.tv_usec;

    d = (T4 - T1) - (T3 - T2);

    sec = d / factor;
    usec = d - sec * factor;

    return {sec, usec};
}

void SNTP::analyze()
{
    timeval receiveTimestamp = now();
    timeval transmitTimeStamp;

    switch (packet.li_vn_mode & 0b00000111)
    {
    case MODE::server:
        //std::cout << "server" << std::cout;

        T2.tv_sec = ntohl(packet.receiveTimestamp.tv_sec);
        T2.tv_usec = ntohl(packet.receiveTimestamp.tv_usec);
        T3.tv_sec = ntohl(packet.transmitTimestamp.tv_sec);
        T3.tv_usec = ntohl(packet.transmitTimestamp.tv_usec);
        T4 = now();

        referenceTimestamp = now();

        break;

    case MODE::client:
        std::cout << "client" << std::cout;

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

        packet.referenceTimestamp.tv_sec = htonl(referenceTimestamp.tv_sec);
        packet.referenceTimestamp.tv_usec = htonl(referenceTimestamp.tv_usec);

        packet.originateTimestamp.tv_sec = htonl(T1.tv_sec);
        packet.originateTimestamp.tv_usec = htonl(T1.tv_usec);

        packet.receiveTimestamp.tv_sec = htonl(receiveTimestamp.tv_sec);
        packet.receiveTimestamp.tv_usec = htonl(receiveTimestamp.tv_usec);

        transmitTimeStamp = now();
        packet.transmitTimestamp.tv_sec = htonl(transmitTimeStamp.tv_sec);
        packet.transmitTimestamp.tv_usec = htonl(transmitTimeStamp.tv_usec);

        break;
    }
}

timeval SNTP::now()
{
    timeval timeStamp;
    uint64_t count = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + epochDiff * factor;

    timeStamp.tv_sec = count / factor;
    timeStamp.tv_usec = count - timeStamp.tv_sec * factor;

    return timeStamp;
}

void SNTP::copy(const uint8_t *src)
{
    memcpy((char *)&packet, src, sizeof(packet));
}