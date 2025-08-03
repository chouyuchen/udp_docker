#include <iostream>
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>

#define SERVER_PORT 9000
#define BUFFER_SIZE 1024
#define TIMEOUT_MS 1000

enum Flags : uint16_t {
    DATA = 0x1,
    ACK  = 0x2,
    NAK  = 0x4,
};

#pragma pack(push,1)
struct PacketHeader {
    uint32_t seq; // 4 bytes , sequence number for payload
    uint32_t ack;  // 4 bytes, acknowledgment number for received packets
    uint16_t flags; // 2 bytes, define the type of packet
    uint16_t len;   // 2 bytes, payload length
};
#pragma pack(pop)


int main() {
    // setup UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1;}

    // set local address and port
    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;  // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 (any address)
    servaddr.sin_port = htons(SERVER_PORT);
    int bind_result = bind(sock, (sockaddr*)&servaddr, sizeof(servaddr)); // bind to local port
    if (bind_result < 0) {
        perror("bind");
        close(sock);
        return 1;
    }

    
    
    uint32_t expected_seq = 0;
    std::cout << "Server up and running on port " << SERVER_PORT <<std::endl;

    while (true) {
        // receive data from any client message
        char buffer[BUFFER_SIZE];
        sockaddr_in clientaddr{};
        socklen_t clientlen = sizeof(clientaddr);
        ssize_t n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientaddr, &clientlen);
        if (n < (ssize_t)sizeof(PacketHeader)) { perror("recvfrom"); continue;}


        auto *header = (PacketHeader*)buffer;
        if(!(header->flags & DATA)) continue; // check if it's a data packet

       
        if(header->seq == expected_seq){
            std::string payload(buffer + sizeof(PacketHeader), header-> len); // extract payload(real data)
            std::cout << "[Server] Received \"" << payload << "\" from seq " <<header->seq << "\"\n"; 
            
            PacketHeader ack_packet{};
            ack_packet.seq = 0; // echo back the same sequence number
            ack_packet.ack = header->seq; // acknowledge the received packet
            ack_packet.flags = Flags::ACK; // set ACK flag
            ack_packet.len = 0; // no payload in ACK packet
            ssize_t sent = sendto(sock, &ack_packet, sizeof(ack_packet), 0, (sockaddr*)&clientaddr, clientlen);            
            expected_seq++; 


        }
        else if (header->seq < expected_seq) {
            std::cout << "[Server] Duplicate packet seq = " << header->seq << " (expect=" << expected_seq << ")\n";
            PacketHeader nak_packet{};
            nak_packet.seq = 0; // echo back the same sequence number
            nak_packet.ack = header->seq; // acknowledge the received packet
            nak_packet.flags = Flags::NAK; // set NAK flag
            nak_packet.len = 0; // no payload in NAK packet
            ssize_t sent = sendto(sock, &nak_packet, sizeof(nak_packet), 0, (sockaddr*)&clientaddr, clientlen);
        }
        else {
            std::cout << "[Server] Out of order packet seq " << header->seq << "(missing " << expected_seq << ")\n";
            PacketHeader nak_packet{};
            nak_packet.seq = 0; // echo back the same sequence number
            nak_packet.ack = header->seq; // acknowledge the received packet
            nak_packet.flags = Flags::NAK; // set NAK flag
            nak_packet.len = 0; // no payload in NAK packet
            ssize_t sent = sendto(sock, &nak_packet, sizeof(nak_packet), 0, (sockaddr*)&clientaddr, clientlen);
        }
        

        // 0.1 Hz
        std::this_thread::sleep_for(std::chrono::seconds(10)); 

    }

    close(sock);

    return 0;
}