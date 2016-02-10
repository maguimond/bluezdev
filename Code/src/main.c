//
// main.cpp
//
// Auteur: Marc-Andre Guimond.
// Créé le 2015/08/18.
// Copyright (C) 2015 par GeniAle.
// Tous droits réservés.
//
// Ce fichier est encodé en UTF-8.
//
// Ce programme sert de point de départ pour tester les fonctions de communication Bluetooth LE.

// Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

// System includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

// Bluetooth lib includes
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

// App includes
#include <utils.h>

#define ATT_CID  4 // 4 ATT_CID??

int main(int argc, char** argv)
{
    // Add BLE address here
    char BLEDestinationAddress[18] = "C5:2A:45:36:E3:A2";

    // Allocate a socket (from btio/btio.c: line 1532)
    int fileDescriptor = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (fileDescriptor < 0)
    {
        ExitOnError(__FUNCTION__, "socket", errno, __FILE__, __LINE__);
    }

    printf("File Descriptor = %d\n");

    // Set connection parameters (from btio/btio.c: l2cap_connect line 346)
    struct sockaddr_l2 SocketAddr = { 0 };
    str2ba(BLEDestinationAddress, &SocketAddr.l2_bdaddr);
    SocketAddr.l2_bdaddr_type = BDADDR_LE_RANDOM;
    SocketAddr.l2_family = AF_BLUETOOTH;
    SocketAddr.l2_cid = htobs(ATT_CID);

    // Connect to server (from btio/btio.c: line 362)
    printf("Connecting...\n");
    int status = connect(fileDescriptor, (struct sockaddr*)&SocketAddr, sizeof(SocketAddr));
    if (status < 0)
    {
        close(fileDescriptor);
        ExitOnError(__FUNCTION__, "connect", errno, __FILE__, __LINE__);
    }
    else
    {
        printf("Connected\n");
    }


    // Test.. compile pas voir README.md reverse engineering #### char-read-uuid ####    
    //epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->fd, &ev);


    // Send a message
    status = write(fileDescriptor, "hello!", 6);
    if( status < 0 )
    {
        close(fileDescriptor);
        ExitOnError(__FUNCTION__, "write", errno, __FILE__, __LINE__);
    }
    else
    {
        printf("write ok \n");
    }

    close(fileDescriptor);
    return 0;
}

