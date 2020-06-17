/** @file   ping.hpp
 *  @brief  Header file for ping program for linux.
 *          Includes all the functions and structures used in the code.
 *  @author Mustafa Siddiqui
 *  @date   05/23/2020
 */

/* Header Guards */
#ifndef PING_H
#define PING_H

/* Include necessary libraries */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>              // getaddrinfo()
#include <netinet/ip.h>         // for n_short and n_time for use in netinet/ip_icmp.h
#include <netinet/ip_icmp.h>    // icmp header
#include <unistd.h>             // getpid()
#include <time.h>
//--//
#include <stdlib.h>
#include <string.h>             // memset()
//--//
#include <iostream>
#include <string>

/* Define constants */
const int PacketSize = 64;      // packet size to be kept as 64 Bytes
const int ReceivalTimeOut = 3;  // timeout delay for receiving packets (3 seconds)
const int TimeToLive = 64;      // ttl value

/** @struct PacketData
 *  @brief  Structure to hold packet to be sent. Includes icmp header and the message.
 */
struct PacketData {
    icmphdr header;
    char message[PacketSize - sizeof(header)];      // want total size to be 64 bytes
};

/** @fn     getIPAddress(char*, addrinfo*&)
 *  @brief  Obtain IP Address in addrinfo from domain name or IP Address stored as char.
 */
int getIPAddress(char* hostName, addrinfo* &result);

/** @fn     initializePacket(int&)
 *  @brief  Set up packet to be sent accordingly for ICMP.
 */
PacketData initializePacket(int &transmitted);

/** @fn     sendPing()
 *  @brief  Function to send an ICMP echo request and receive an echo reply. Makes use of the
 *          initializePacket() function to create a packet and sends to the address pointed to
 *          by pDestinationAddr (a pointer). Displays RTT (Round Trip Time in milliseconds)
 *          and packet success percentage after every ping.
 */
int sendPing(int socketFD, int &transmitted, sockaddr_in* pDestinationAddr, int &success, int &failure);

#endif