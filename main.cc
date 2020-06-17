/** @file   main.cc
 *  @brief  C++ implementation for a ping program for linux.
 *
 *          Compile: g++ main.cc ping.cc -o ping
 *
 *          OR
 *
 *          Compile: make
 *
 *          Run: sudo ./ping <domain_name/IP_address>
 *
 *  @author Mustafa Siddiqui
 *  @date   05/23/2020
 */

/* Include necessary libraries */
#include <arpa/inet.h>      // inet_ntop()
#include <netinet/in.h>
//--//
#include "ping.h"
//--//
#include <iostream>
#include <string>

/** @fn     main(int, char**)
 *  @brief  Main function of the program. Uses functions from ping.hpp to
 *          implement ping.
 */
int main(int argc, char* argv[]) {

    // validate command line arguments
    if (argc != 2) {
        std::cout << "Usage: sudo " << argv[0] << " <domain_name/IP_address>" << std::endl;
        exit(EXIT_FAILURE);
    }

    // variables needed to store IP Address
    addrinfo* result;
    sockaddr_in* pDestinationAddr;
    char ipString[INET_ADDRSTRLEN];

    // get IP Address and store in 'result' (passed by reference)
    if (getIPAddress(argv[1], result) != 0) {
        std::cout << "Invalid IP Address. Terminating ...\n";
        exit(EXIT_FAILURE);
    }
    else {
        pDestinationAddr = (sockaddr_in*)result->ai_addr;                           // get struct from resulting linked list
        void* address;
        address = &pDestinationAddr->sin_addr;                                      // store IP Address
        inet_ntop(result->ai_family, address, ipString, sizeof(ipString));          // convert binary IP to string
        std::cout << "IP: " << ipString << std::endl;
    }

    // get appropriate socket file descriptor
    int socketFD = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socketFD < 0) {
        std::cout << "Socket File Descriptor creation failed.\n";
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "Socket File Descriptor: " << socketFD << std::endl;
    }

    // set socket option to TTL (time to live)
    if (setsockopt(socketFD, IPPROTO_IP, IP_TTL, &TimeToLive, sizeof(TimeToLive)) != 0) {
        std::cout << "Setting Socket Options failed.\n";
        exit(EXIT_FAILURE);
    }
    else {  // for debugging purposes
        std::cout << "Socket Option Set to TTL.\n";
    }

    // declare and initialize struct
    timeval TimeOut = {ReceivalTimeOut, 0};

    // set timeout of receival
    if (setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&TimeOut, sizeof(TimeOut)) != 0) {
        std::cout << "Setting Timeout for Receival failed.\n";
        exit(EXIT_FAILURE);
    }
    else {  // for debugging purposes
        std::cout << "Timeout for receival set.\n";
    }

    // free linked list before infinite loop
    freeaddrinfo(result);

    // variables to track packets
    int transmitted = 0;

    std::cout << "\n-----\n\n";

    int success = 0, failure = 0;

    // ping in infinite loop
    for (;;) {
        int flag = sendPing(socketFD, transmitted, pDestinationAddr, success, failure);

        if (flag != -1) {
            // add delay of 0.5 sec
            usleep(500000);
        }
    }

    // never reached
    close(socketFD);

    return 0;
}
