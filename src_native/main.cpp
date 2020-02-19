// http://www.c-worker.ch/tuts/udp.php

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include "SNTP.hpp"

//Prototypen
int startWinsock(void);

int main()
{
    long rc;
    SOCKET s;
    SOCKADDR_IN addr;
    SOCKADDR_IN remoteAddr;
    int remoteAddrLen = sizeof(SOCKADDR_IN);
    SNTP sntp = SNTP();

    sntp.clientPacketPrepare();

    rc = startWinsock();
    if (rc != 0)
    {
        printf("Fehler: startWinsock, fehler code: %d\n", rc);
        return 1;
    }
    else
    {
        printf("Winsock gestartet!\n");
    }

    //UDP Socket erstellen
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET)
    {
        printf("Fehler: Der Socket konnte nicht erstellt werden, fehler code: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("UDP Socket erstellt!\n");
    }

    // addr vorbereiten
    addr.sin_family = AF_INET;
    addr.sin_port = htons(123);
    // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_addr.s_addr = inet_addr("153.88.71.60");

    rc = sendto(s, (char *)&sntp.ntpPacket, sizeof(sntp.ntpPacket), 0, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN));
    if (rc == SOCKET_ERROR)
    {
        printf("Fehler: sendto, fehler code: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("%d Bytes gesendet!\n", rc);
    }
    sntp.printPacket();

    rc = recvfrom(s, (char *)&sntp.ntpPacket, sizeof(sntp.ntpPacket), 0, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
    if (rc == SOCKET_ERROR)
    {
        printf("Fehler: recvfrom, fehler code: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("%d Bytes empfangen!\n", rc);
        sntp.printPacket();
    }

    std::cout << "done" << std::endl;

    return 0;
}

int startWinsock(void)
{
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 0), &wsa);
}