#ifndef PING_CPP
#define PING_CPP

#include "ping.hpp"

/* Calculate Checksum */
u_short checksum(u_short *addr, int len) {

	int nleft = len;
	u_short *w = addr;
	int sum = 0;
	u_short answer = 0;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1) {
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			        /* add carry */
	answer = ~sum;				        /* truncate to 16 bits */

	return(answer);
}

/* IP Address */
int getIPAddress(char* hostName, addrinfo*& result) {

    addrinfo temp = {0};
    temp.ai_family = AF_INET;          // want IPv4 
    temp.ai_socktype = SOCK_DGRAM;     // set socket type -> datagram
    temp.ai_flags = AI_PASSIVE;        // fill in IP automatically

    //std::cout << getaddrinfo(hostName, PORT_NO, &temp, &result) << '\n';
    return (getaddrinfo(hostName, PORT_NO, &temp, &result));    // get & validate given ip address
}

/* Socket File Descriptor */
int getSocketFileDescriptor(addrinfo*& result) {
    
    //return (socket(result->ai_family, result->ai_socktype, result->ai_protocol));
    return socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
}

/* Set up Packet */
pingPacket initializePacket(int& transmitted) {
    
    pingPacket packet = {};
    //memset(packet, 0, sizeof(packet));
    packet.header.icmp_type = ICMP_ECHO;                            // set ICMP Type to Echo
    packet.header.icmp_code = 0;
    packet.header.icmp_cksum = checksum((u_short*)&packet, sizeof(packet));   // initialize checksum
    
    packet.header.icmp_hun.ih_idseq.icd_seq = transmitted++;        // set seq number
    packet.header.icmp_hun.ih_idseq.icd_id = getpid() & 0xFFFF;     // set id (ICMP field is 16 bits)
    
    //  fill up message
    memset(&packet.message, '0', sizeof(packet.message));
    packet.message[PACKET_SIZE - sizeof(packet.header) - 1] = '\0';

    return packet;
}

/* send ECHO REQUEST and receive ECHO REPLY */
void sendPing(int socketFD, int& transmitted, timespec& startTime, timespec& endTime, sockaddr_in* pingAddress, std::string ipString) {
    
    /* send ping */
    pingPacket packet = initializePacket(transmitted);
    // debug statement below
    //std::cout << "Packet Details: " << (int)packet.header.icmp_type << ", " << (int)packet.header.icmp_code << std::endl;
    //std::cout << "Packet message: " << packet.message << '\n';
    clock_gettime(CLOCK_MONOTONIC, &startTime);     // get start time
    
    int pingBytesSent = sendto(socketFD, (char*)&packet, PACKET_SIZE, 0, (sockaddr*)pingAddress, sizeof(sockaddr));
    if (pingBytesSent == -1) {
        std::cout << "Packet Sending Failed.\n";
    }
    else {
        std::cout << pingBytesSent << " bytes sent\n";

        /* receive ping */
        sockaddr rAddress = {};
        socklen_t addrLength = (socklen_t)sizeof(rAddress);
        int received = 0;
        u_char receivedPacket;

        int pingBytesReceived = recvfrom(socketFD, &receivedPacket, sizeof(receivedPacket), 0, &rAddress, &addrLength);
        if (pingBytesReceived == -1) {
            std::cout << "Packet Receival Failed.\n";
            std::cout << "Error Code: " << errno << '\n';
        }
        else {
            clock_gettime(CLOCK_MONOTONIC, &endTime);   // get time when data received
            /* calculate rtt in milliseconds */
            double elapsedTime = (double)(endTime.tv_nsec - startTime.tv_nsec) / 1000000.0;
            long double rtt_msec = (endTime.tv_sec - startTime.tv_sec) * 1000.0 + elapsedTime;
            received++;

            std::cout << pingBytesReceived << " bytes received\n";

            /* Output Ping Details */
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
}

#endif