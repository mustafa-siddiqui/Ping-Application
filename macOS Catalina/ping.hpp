/** @file   ping.hpp
 *  @brief  Header file for ping program for MACOS X.
 *          Includes all the functions and structures used in code.
 *  @author Mustafa Siddiqui
 *  @date   04/19/20
 */

/* Header Guards */
#ifndef PING_HPP
#define PING_HPP

/* Include necessary libraries */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>              // getaddrinfo()
#include <time.h>
#include <netinet/ip.h>         // for n_short and n_time for use in netinet/ip_icmp.h
#include <netinet/ip_icmp.h>    // icmp struct
#include <unistd.h>             // getpid()
// -- //
#include <string>               
#include <iostream>
#include <stdlib.h>

/* MACROS to improve readability */
#define PACKET_SIZE 64          // Packet Size is 64 Bytes
#define PORT_NO 0               // Port Number 
#define RECV_TIMEOUT 3          // Timeout Delay for receiving packets (3 seconds)

/** @struct pingPacket
 *  @brief  Structure to hold packet to be sent. Includes icmp header and the message.
 */
struct pingPacket {

    icmp header;
    char message[PACKET_SIZE - sizeof(icmp)];   // want total packet size to be 64 Bytes
};

/** @fn     checksum(u_short*, int)
 *  @brief  Function used to calculate checksum.
 *          // USED FROM APPLE //
 */ 
u_short checksum(u_short* addr, int len);

/** @fn     getIPAddress(char*, addrinfo*&)
 *  @brief  Obtain IP address in addrinfo* from domain name or IP Address stored as char*.
 */
int getIPAddress(char* hostName, addrinfo*& result);

/** @fn     getSocketFileDescriptor(addrinfo*)
 *  @brief  Get appropriate socket file descriptor for ICMP transmission.
 */
int getSocketFileDescriptor(addrinfo*& result);

/** @fn     initializePacket(int&)
 *  @brief  Set up packet to be sent accordingly for ICMP.
 */
pingPacket initializePacket(int& transmitted);

/** @fn     sendPing(int, int&, timespec&, timespec&, sockaddr_in*, std::string)
 *  @brief  Call initializePacket() and set up packet, get start and end time, and 
 *          send ECHO REQUEST and receive ECHO REPLY.
 *          Report RTT (Round Trip Time) & Packet Loss OR errors encountered during 
 *          transmission (if any).
 */
int sendPing(int socketFD, int& transmitted, timespec& startTime, timespec& endTime, sockaddr_in* pingAddress, std::string ipString);


#endif
