#include "udp_server.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include <cstring>

udp_server::UDPServer::UDPServer(const uint16_t port) 
    : expected_seq_(0) { // initialize expected sequence number
    // Create UDP socket
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ < 0) {
        perror("socket");
    }

    // Set up server address
    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY; // Bind to any address
    servaddr.sin_port = htons(port); // Convert port to network byte order

    // Bind the socket to the address and port
    if (bind(sock_, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
    }
    std::cout << "Server up and running on port " << port << std::endl;
}


udp_server::UDPServer::~UDPServer(){
    close(sock_);
}


void udp_server::UDPServer::run() {
    while (true) {
        char buffer[BUFFER_SIZE];
        sockaddr_in clientaddr{};
        socklen_t clientlen = sizeof(clientaddr);
        
        // Receive data from any client
        ssize_t n = recvfrom(sock_, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientaddr, &clientlen);
        if (n < 0) { 
            perror("recvfrom");
            break;
        }

        handlePacket(buffer, n, clientaddr, clientlen);
        std::this_thread::sleep_for(std::chrono::seconds(10));  // exchange as 0.1 Hz
    }
}


void udp_server::UDPServer::handlePacket(const char* buffer, ssize_t n,
                      const sockaddr_in& client, socklen_t clientlen)
{
    auto *header = reinterpret_cast<const udp_packet::PacketHeader*>(buffer); // casting the front buffer to PacketHeader
    if(!(header->flags == static_cast<uint16_t>(udp_packet::Flags::DATA))) return; // check if it's a data packet

    if (header->seq == expected_seq_){
        std::string payload(buffer + sizeof(udp_packet::PacketHeader), header->len);
        std::cout << "[Server] Received \"" << payload << "\" from seq " <<header->seq << "\"\n";
        sendResponse(header->seq, udp_packet::Flags::ACK, client, clientlen);
        expected_seq_++; // increment expected sequence number
    }
    else if (header->seq < expected_seq_){
        std::cout << "[Server] Duplicate packet seq = " << header->seq << " (expect=" << expected_seq_ << ")\n";
        sendResponse(header->seq, udp_packet::Flags::NAK, client, clientlen);
    }
    else {
        std::cout << "[Server] Out of order packet seq " << header->seq << "(missing " << expected_seq_ << ")\n";
        sendResponse(header->seq, udp_packet::Flags::NAK, client, clientlen);
    }

}


void udp_server::UDPServer::sendResponse(uint32_t ack_num, udp_packet::Flags f,
                     const sockaddr_in& client, socklen_t clientlen) {
    udp_packet::PacketHeader response{};
    response.seq = 0; // echo back the same sequence number
    response.ack_num = ack_num; // acknowledge the received packet
    response.flags = static_cast<uint16_t>(f); // set appropriate flag
    response.len = 0; // no payload in ACK/NAK packet

    ssize_t sent = sendto(sock_, &response, sizeof(response), 0, (sockaddr*)&client, clientlen);
    if (sent < 0) {
        perror("sendto");
    }
}