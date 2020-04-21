/** @file main.cpp
 *  @brief 
 *  @author Mustafa Siddiqui
 *  @date 4/17/20
 */

#include <iostream>
#include <string>
#include "ping.hpp"
#include <arpa/inet.h>  // inet_ntop()
#include <netinet/in.h>

int main(int argc, char* argv[]) {

    /* validate arguments */
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <IP Address>" << std::endl;
        exit(EXIT_FAILURE);
    }

    addrinfo* result;
    char ipString[INET_ADDRSTRLEN];                                         // character array to hold ip address as a string
    int ipStatus = getIPAddress(argv[1], result);
    if (ipStatus != 0) {                                                    // if some error (result is passed by reference here)
        std::cout << "Invalid IP Address.\nTerminating ..." << std::endl;
        return 1;
    }
    else {
        
        sockaddr_in* ipv4 = (sockaddr_in*)result->ai_addr;                  // get struct from resulting link list
        void* address;
        address = &ipv4->sin_addr;                                          // store ip address
        inet_ntop(result->ai_family, address, ipString, sizeof(ipString));  // convert binary IP to string
        std::cout << ipString << std::endl;
    }

    int socketFD = getSocketFileDescriptor(result);
    if (socketFD < 0) {
        std::cout << "Socket File Descriptor Not Received\n";
        return 1;
    }
    else {
        std::cout << "Socket File Descriptor Received: " << socketFD << std::endl;
    }

    int transmitted = 0;
    timespec startTime, endTime;
    timeval tvOut = {RECV_TIMEOUT, 0};      // initialize struct
    sockaddr_in* pingAddress = (sockaddr_in*)result->ai_addr;

    /* set socket option to TTL */
    int ttl_value = 64;
    if (setsockopt(socketFD, IPPROTO_IP, IP_TTL, &ttl_value, sizeof(ttl_value)) != 0) {

        std::cout << "Setting Socket Options Failed.\n";
        exit(EXIT_FAILURE);
    }
    else {  // for debugging purposes
        std::cout << "Socket Option Set to TTL!\n";
    }
    /* set timeout of receival */
    /*if (setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tvOut, sizeof(tvOut)) != 0) {
        std::cout << "Setting Timeout for Receival Failed.\n";
        exit(EXIT_FAILURE);
    }
    else {  // for debugging purposes
        std::cout << "Time of RECV set\n";
    }*/

    free(result);       // free linked list before infinite loop

    for (;;) {          // ping in infinite loop

        sendPing(socketFD, transmitted, startTime, endTime, pingAddress, ipString);
        std::cout << std::endl;
    }

    close(socketFD);    // close socket        

    return 0;
}