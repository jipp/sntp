#include "SNTP.hpp"

void SNTP::print()
{
    std::cout << std::endl;
    std::cout << "li_vn_mode: 0b" << std::bitset<8>(packet.li_vn_mode) << std::endl;
    std::cout << "stratum: " << static_cast<uint16_t>(packet.stratum) << std::endl;
    std::cout << "poll: " << static_cast<uint16_t>(packet.poll) << std::endl;
    std::cout << "precision: " << static_cast<uint16_t>(packet.precision) << std::endl;
    std::cout << "rootDelay: " << ntohl(packet.rootDelay) << std::endl;
    std::cout << "rootDispersion: " << ntohl(packet.rootDispersion) << std::endl;
    std::cout << "ReferenceIdentifier: " << packet.referenceIdentifier[0] << packet.referenceIdentifier[1] << packet.referenceIdentifier[2] << packet.referenceIdentifier[3] << std::endl;

    std::cout << "referenceTimestamp_s: " << ntohl(packet.referenceTimestamp.tv_sec) << std::endl;
    std::cout << "referenceTimestamp_f: " << deFrac(ntohl(packet.referenceTimestamp.tv_usec)) << std::endl;
    std::cout << "originateTimestamp_s: " << ntohl(packet.originateTimestamp.tv_sec) << std::endl;
    std::cout << "originateTimestamp_f: " << deFrac(ntohl(packet.originateTimestamp.tv_usec)) << std::endl;
    std::cout << "T1_s: " << T1.tv_sec << std::endl;
    std::cout << "T1_f: " << T1.tv_usec << std::endl;

    std::cout << "T2_s / receiveTimestamp_s: " << ntohl(packet.receiveTimestamp.tv_sec) << std::endl;
    std::cout << "T2_f / receiveTimestamp_f: " << deFrac(ntohl(packet.receiveTimestamp.tv_usec)) << std::endl;

    std::cout << "T3_s / transmitTimestamp_s: " << ntohl(packet.transmitTimestamp.tv_sec) << std::endl;
    std::cout << "T3_f / transmitTimestamp_f: " << deFrac(ntohl(packet.transmitTimestamp.tv_usec)) << std::endl;

    std::cout << "T4_s: " << T4.tv_sec << std::endl;
    std::cout << "T4_f: " << T4.tv_usec << std::endl;

    std::cout << std::endl;
}

void SNTP::prepareClient()
{
    T1 = now();

    packet.li_vn_mode = (LI::noWarning << 6) | (VERSION::v3 << 3) | MODE::client;

    // packet.stratum = 0;
    // packet.poll = 0;
    // packet.precision = 0;

    // packet.rootDelay = 0;
    // packet.rootDispersion = 0;
    // packet.referenceIdentifier[0] = ' ';
    // packet.referenceIdentifier[1] = ' ';
    // packet.referenceIdentifier[2] = ' ';
    // packet.referenceIdentifier[3] = ' ';

    packet.referenceTimestamp.tv_sec = htonl(referenceTimestamp.tv_sec);
    packet.referenceTimestamp.tv_usec = htonl(frac(referenceTimestamp.tv_usec));

    packet.originateTimestamp.tv_sec = htonl(T1.tv_sec);
    packet.originateTimestamp.tv_usec = htonl(frac(T1.tv_usec));

    // packet.receiveTimestamp.tv_sec = 0;
    // packet.receiveTimestamp.tv_usec = 0;
    // packet.transmitTimestamp.tv_sec = 0;
    // packet.transmitTimestamp.tv_usec = 0;
}

timeval SNTP::getOffset()
{
    int64_t t = 0;
    int32_t sec = 0;
    int32_t usec = 0;
    uint64_t T1 = this->T1.tv_sec * factorFractions + this->T1.tv_usec;
    uint64_t T2 = this->T2.tv_sec * factorFractions + this->T2.tv_usec;
    uint64_t T3 = this->T3.tv_sec * factorFractions + this->T3.tv_usec;
    uint64_t T4 = this->T4.tv_sec * factorFractions + this->T4.tv_usec;

    t = static_cast<int64_t>((T2 - T1) + (T3 - T4)) / 2;

    sec = t / static_cast<int64_t>(factorFractions);
    usec = t - sec * static_cast<int64_t>(factorFractions);

    return {sec, usec};
}

timeval SNTP::getDelay()
{
    int64_t d = 0;
    int32_t sec = 0;
    int32_t usec = 0;
    uint64_t T1 = this->T1.tv_sec * factorFractions + this->T1.tv_usec;
    uint64_t T2 = this->T2.tv_sec * factorFractions + this->T2.tv_usec;
    uint64_t T3 = this->T3.tv_sec * factorFractions + this->T3.tv_usec;
    uint64_t T4 = this->T4.tv_sec * factorFractions + this->T4.tv_usec;

    d = (T4 - T1) - (T3 - T2);

    sec = d / static_cast<int64_t>(factorFractions);
    usec = d - sec * static_cast<int64_t>(factorFractions);

    return {sec, usec};
}

void SNTP::analyze()
{
    tv receiveTimestamp = now();
    tv transmitTimeStamp;

    switch (packet.li_vn_mode & 0b00000111)
    {
    case MODE::server:
        T2.tv_sec = ntohl(packet.receiveTimestamp.tv_sec);
        T2.tv_usec = deFrac(ntohl(packet.receiveTimestamp.tv_usec));
        T3.tv_sec = ntohl(packet.transmitTimestamp.tv_sec);
        T3.tv_usec = deFrac(ntohl(packet.transmitTimestamp.tv_usec));
        T4 = now();

        referenceTimestamp = now();

        break;

    case MODE::client:
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
        packet.referenceTimestamp.tv_usec = htonl(frac(referenceTimestamp.tv_usec));

        packet.originateTimestamp.tv_sec = htonl(T1.tv_sec);
        packet.originateTimestamp.tv_usec = htonl(frac(T1.tv_usec));

        packet.receiveTimestamp.tv_sec = htonl(receiveTimestamp.tv_sec);
        packet.receiveTimestamp.tv_usec = htonl(frac(receiveTimestamp.tv_usec));

        transmitTimeStamp = now();
        packet.transmitTimestamp.tv_sec = htonl(transmitTimeStamp.tv_sec);
        packet.transmitTimestamp.tv_usec = htonl(frac(transmitTimeStamp.tv_usec));

        break;
    }
}

tv SNTP::now()
{
    tv timeStamp;
    uint64_t count = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + factorFractions * epochDiff;

    timeStamp.tv_sec = count / factorFractions;
    timeStamp.tv_usec = count - timeStamp.tv_sec * factorFractions;

    return timeStamp;
}

void SNTP::copy(const uint8_t *src)
{
    memcpy(reinterpret_cast<char *>(&packet), src, sizeof(packet));
}

uint32_t SNTP::deFrac(uint32_t frac)
{
    return (static_cast<uint64_t>(frac) * factorFractions) >> 32;
}

uint32_t SNTP::frac(uint32_t deFrac)
{
    return (static_cast<uint64_t>(deFrac) << 32) / factorFractions;
}