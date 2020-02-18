#include <stdio.h>

// http://www.c-worker.ch/tuts/udp.php

#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#include "SNTP.hpp"

       int
       startWinsock(void);

int main()
{

    long rc;
    SOCKET s;

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

    return 0;
}

int startWinsock(void)
{
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 0), &wsa);
}