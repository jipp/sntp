// http://www.c-worker.ch/tuts/udp.php

#include <cstdio>
#include <cstdlib>

#include "SNTP.hpp"

//Prototypen
int startWinsock(void);

int main()
{
    int rc = 0;
    SOCKET s;
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

    // addr vorbereiten
    addr.sin_family = AF_INET;
    addr.sin_port = htons(123);
    addr.sin_addr.s_addr = inet_addr("153.88.71.60"); // ///
    // addr.sin_addr.s_addr = inet_addr("94.130.184.193"); // ntp.pool
    // addr.sin_addr.s_addr = inet_addr("192.168.178.1");  // fritz.box
    // addr.sin_addr.s_addr = inet_addr("192.168.4.1");    // esp8266 AP

    sntp.prepareClient();
    sntp.print();

    rc = sendto(s, (char *)&sntp.packet, sizeof(sntp.packet), 0, (SOCKADDR *)&addr, sizeof(SOCKADDR_IN));

    if (rc == SOCKET_ERROR)
    {
        printf("Fehler: sendto, fehler code: %d\n", WSAGetLastError());

        return 1;
    }
    else
    {
        printf("%d Bytes gesendet!\n", rc);
    }

    rc = recvfrom(s, (char *)&sntp.packet, sizeof(sntp.packet), 0, (SOCKADDR *)&remoteAddr, &remoteAddrLen);

    if (rc == SOCKET_ERROR)
    {
        printf("Fehler: recvfrom, fehler code: %d\n", WSAGetLastError());

        return 1;
    }
    else
    {
        printf("%d Bytes empfangen!\n", rc);
        sntp.analyze();
        sntp.print();
        std::cout << "offset: " << sntp.getOffset().tv_sec << " sec; fragments: " << sntp.getOffset().tv_usec << " usec" << std::endl;
        std::cout << "delay: " << sntp.getDelay().tv_sec << " sec; fragments: " << sntp.getDelay().tv_usec << " usec" << std::endl;
    }

    std::cout << "done" << std::endl;

    return 0;
}

int startWinsock(void)
{
    WSADATA wsa;

    return WSAStartup(MAKEWORD(2, 0), &wsa);
}