#ifndef PING_CC
#define PING_CC

/* Include necessary libraries */
#include <arpa/inet.h>          // inet_ntop()
//--//
#include "ping.h"

/* IP Address */
int getIPAddress(char* hostName, addrinfo* &result) {
    addrinfo tempStruct = {0};

    tempStruct.ai_family = AF_INET;             // want IPv4
    //tempStruct.ai_socktype = SOCK_DGRAM;      // set socket type to datagram
    //tempStruct.ai_flags = AI_PASSIVE;         // fill in IP automatically

    // get and validate IP address
    return (getaddrinfo(hostName, "http", &tempStruct, &result));
}

/* Set up packet */
PacketData initializePacket(int &transmitted) {
    PacketData packet = {};

    packet.header.type = ICMP_ECHO;                 // set ICMP type to Echo
    packet.header.un.echo.id = getpid() & 0xFFFF;   // set id (ICMP field is 16 bits)
    packet.header.checksum = 0;                     // fixed checksum because data is unchanging
    packet.header.un.echo.sequence = transmitted++;

    // fill up message
    memset(&packet.message, '0', sizeof(packet.message));
    packet.message[PacketSize - sizeof(packet.header) - 1] = '\0';

    return packet;
}

/* Send echo request and reply */
int sendPing(int socketFD, int &transmitted, sockaddr_in* pDestinationAddr, int &success, int &failure) {
    int bytesSent = 0, bytesReceived = 0;
    timespec StartTime, EndTime;

    // initialize packet to be sent
    PacketData packet = initializePacket(transmitted);

    // get start time
    clock_gettime(CLOCK_MONOTONIC, &StartTime);

    // send echo request
    bytesSent = sendto(socketFD,                                                            // socket file descriptor
                      (char*)&packet, PacketSize,                                           // packet and size
                      0,                                                                    // flags
                      (sockaddr*)pDestinationAddr, (socklen_t)sizeof(*pDestinationAddr));   // destination address and size

    if (bytesSent == -1) {
        std::cout << "Packet sending failed.\n";
        std::cout << "Error Code: " << errno << std::endl;
    }
    else {
        std::cout << bytesSent << " bytes sent.\n";

        // variables needed to store IP address of echo reply sender
        sockaddr_in* pReceiveFromAddr = new sockaddr_in;
        socklen_t addrLength = (socklen_t)sizeof(*pReceiveFromAddr);
        u_char receivedPacket;

        // receive echo reply
        bytesReceived = recvfrom(socketFD,                                                  // socket file descriptor
                                &receivedPacket, sizeof(receivedPacket),                    // packet received and size
                                0,                                                          // flags
                                (sockaddr*)pReceiveFromAddr, &addrLength);                  // address of echo reply sender and size

        if (bytesReceived == -1) {
            failure++;
            std::cout << "Packet receival failed.\n";
            std::cout << "Error Code: " << errno << std::endl;
        }
        else {
            success++;
            clock_gettime(CLOCK_MONOTONIC, &EndTime);

            // calculate round trip time (RTT)
            double rttTime = (EndTime.tv_sec - StartTime.tv_sec) / 1000.0 +
                             (EndTime.tv_nsec - StartTime.tv_nsec) * 1000000.0;

            // convert IP address received to string
            char recvString[INET_ADDRSTRLEN];
            void* address;
            sockaddr_in* temp = pReceiveFromAddr;
            address = &temp->sin_addr;                                                  // store IP address

            // convert binary IP to string
            inet_ntop(AF_INET, address, recvString, sizeof(recvString));
            std::cout << "RECV IP: " << recvString << std::endl;

            // output details
            std::cout << sizeof(receivedPacket) + sizeof(*pReceiveFromAddr) << " bytes received from " << recvString
                      << ": icmp_seq=" << transmitted << " ttl=64 time=" << rttTime << " ms\n";
        }
    }

    // avoid divide by zero error
    if (bytesSent > 0 && success > 0) {
        std::cout << (failure / (failure + success)) * 100 << "% packet loss so far.\n";
    }

    return bytesReceived;
}

#endif
