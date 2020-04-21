/* Header Guards */
#ifndef PING_HPP
#define PING_HPP

/* Include necessary libraries */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>              // getaddrinfo()
#include <time.h>
// -- //
#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <netinet/ip.h>         // for n_short and n_time for use in netinet/ip_icmp.h
#include <netinet/ip_icmp.h>    // icmp struct
#include <unistd.h>             // getpid()
#include <fcntl.h>              // fcntl()


#define PACKET_SIZE 64      // Packet Size is 64 Bytes
#define PORT_NO 0           // Port Number 
#define RECV_TIMEOUT 3      // Timeout Delay for receiving packets (3 seconds)

struct pingPacket {

    icmp header;
    char message[PACKET_SIZE - sizeof(icmp)];   // want total packet size to be 64 Bytes
};

// Calculating the Check Sum
/* Used from geeksforgeeks.org */
unsigned short checksum(void* b, int len) {
    
    unsigned short* buf = (unsigned short*)b; 
    unsigned int sum=0; 
    unsigned short result; 
  
    for ( sum = 0; len > 1; len -= 2 ) 
        sum += *buf++; 
    if ( len == 1 ) 
        sum += *(unsigned char*)buf; 
    sum = (sum >> 16) + (sum & 0xFFFF); 
    sum += (sum >> 16); 
    result = ~sum;

    return result; 
} 

int getIPAddress(char* hostName, addrinfo*& result) {

    addrinfo temp = {0};
    temp.ai_family = AF_INET;          // want IPv4 
    temp.ai_socktype = SOCK_DGRAM;       // set socket type -> datagram
    temp.ai_flags = AI_PASSIVE;        // fill in IP automatically

    //std::cout << getaddrinfo(hostName, PORT_NO, &temp, &result) << '\n';
    return (getaddrinfo(hostName, PORT_NO, &temp, &result));    // get & validate given ip address
}

int getSocketFileDescriptor(addrinfo*& result) {
    
    //return (socket(result->ai_family, result->ai_socktype, result->ai_protocol));
    return socket(PF_INET, SOCK_DGRAM, IPPROTO_ICMP);
}

pingPacket initializePacket(int& transmitted) {
    
    pingPacket packet = {};
    //memset(&packet, 0, sizeof(packet));
    packet.header.icmp_type = ICMP_ECHO;                            // set ICMP Type to Echo
    packet.header.icmp_code = 0;
    packet.header.icmp_cksum = checksum(&packet, sizeof(packet));   // initialize checksum
    packet.header.icmp_hun.ih_idseq.icd_seq = transmitted++;        // set seq number
    packet.header.icmp_hun.ih_idseq.icd_id = getpid() & 0xFFFF;     // set id (ICMP field is 16 bits)
    
    //  fill up message
    /*for (int i = 0; i < (int)sizeof(packet.message); i++) {

        packet.message[i] = '0';
    }*/
    memset(&packet.message, '0', sizeof(packet.message));
    packet.message[PACKET_SIZE - sizeof(packet.header) - 1] = '\0';

    return packet;
}


void sendPing(int socketFD, int& transmitted, timespec& startTime, timespec& endTime, sockaddr_in* pingAddress, std::string ipString) {
    
    /* send ping */
    pingPacket packet = initializePacket(transmitted);
    // debug statement below
    std::cout << "Packet Details: " << (int)packet.header.icmp_type << ", " << (int)packet.header.icmp_code << std::endl;
    std::cout << "Packet message: " << packet.message << '\n';
    clock_gettime(CLOCK_MONOTONIC, &startTime);
    
    int pingBytesSent = sendto(socketFD, &packet, sizeof(packet), 0, (sockaddr*)pingAddress, sizeof(pingAddress));
    if (pingBytesSent == -1) {
        std::cout << "Packet Sending Failed.\n";
    }
    else {
        std::cout << pingBytesSent << " bytes sent\n";
    }

    /* receive ping */
    sockaddr* rAddress;
    socklen_t addrLength = (socklen_t)sizeof(*rAddress);
    int received = 0;
    u_char receivedPacket;

    /*
    int flags = fcntl(socketFD, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(socketFD, F_SETFL, flags);    */

    int pingBytesReceived = recvfrom(socketFD, &receivedPacket, sizeof(receivedPacket), 0, rAddress, &addrLength);
    if (pingBytesReceived == -1) {
        std::cout << "Packet Receival Failed.\n";
        std::cout << errno << '\n';
    }
    else {
        clock_gettime(CLOCK_MONOTONIC, &endTime);
        double elapsedTime = (double)(endTime.tv_nsec - startTime.tv_nsec) / 1000000.0;
        long double rtt_msec = (endTime.tv_sec - startTime.tv_sec) * 1000.0 + elapsedTime;
        received++;

        std::cout << pingBytesReceived << " bytes received\n";

        if (packet.header.icmp_type == 8 && packet.header.icmp_code == 0) {

            std::cout << PACKET_SIZE << " bytes from " << ipString /*enter the dstaddr here */<< " icmp_seq=" << transmitted
                        << " rtt: " << rtt_msec << " ms" << std::endl;
            std::cout << ((transmitted - received) / transmitted) * 100 << " % packet loss\n\n";
        }
        else {
            std::cout << "Error: Packet Received with ICMP type: " << (int)packet.header.icmp_type
                        << " and code " << (int)packet.header.icmp_code << std::endl;
        }
    }
}


#endif
