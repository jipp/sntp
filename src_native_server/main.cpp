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
    char buf[256];
    char buf2[300];
    SOCKADDR_IN addr;
    SOCKADDR_IN remoteAddr;
    int remoteAddrLen = sizeof(SOCKADDR_IN);
    SNTP sntp = SNTP();

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

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = ADDR_ANY;

    rc = bind(s, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN));
    if (rc == SOCKET_ERROR)
    {
        printf("Fehler: bind, fehler code: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("Socket an Port 1234 gebunden\n");
    }

    while (1)
    {
        // rc = recvfrom(s, buf, 256, 0, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
        rc = recvfrom(s, (char *)&sntp.ntpPacket, sizeof(sntp.ntpPacket), 0, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
        if (rc == SOCKET_ERROR)
        {
            printf("Fehler: recvfrom, fehler code: %d\n", WSAGetLastError());
            return 1;
        }
        else
        {
            printf("%d Bytes empfangen!\n", rc);
            buf[rc] = '\0';
        }
        // printf("Empfangene Daten: %s\n", buf);
        sntp.printPacket();

        //Antworten
        sprintf(buf2, "Du mich auch %s", buf);
        rc = sendto(s, buf2, strlen(buf2), 0, (SOCKADDR *)&remoteAddr, remoteAddrLen);
        if (rc == SOCKET_ERROR)
        {
            printf("Fehler: sendto, fehler code: %d\n", WSAGetLastError());
            return 1;
        }
        else
        {
            printf("%d Bytes gesendet!\n", rc);
        }
    }
    return 0;
}

int startWinsock(void)
{
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 0), &wsa);
}