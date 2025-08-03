#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "udp_client.hpp"


udp_client::UDPClient::UDPClient(const std::string& host, uint16_t port) 
    : seq_(0) {
    // Create UDP socket
    sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ < 0) {perror("socket");}

    // Resolve server address
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
        perror("getaddrinfo");
        close(sock_);
    }
    
    servaddr_ = *(sockaddr_in*)res->ai_addr;
    freeaddrinfo(res); // Free addrinfo structure
    std::cout << "Client up and running "<< std::endl;
}


udp_client::UDPClient::~UDPClient() {
    close(sock_);
}


void udp_client::UDPClient::run(){
    while(true){
        std::string payload = "msg#" + std::to_string(seq_);

        bool done = false;
        while(!done) {
            if(!sendData(payload)){ // failed to send data
                std::this_thread::sleep_for(std::chrono::milliseconds(100));// retry after 100 ms
                continue;
            }

            udp_packet::Flags response = receiveResponse();
            if(f == udp_packet::Flags::ACK) {
                std::cout << "[Client] Received ACK for seq=" << seq_ << std::endl;
                done = true; // exit loop on successful ACK
            } else if (f == udp_packet::Flags::NAK) {
                std::cout << "[Client] Received NAK for seq=" << seq_ << std::endl;
            } else {
                std::cerr << "[Client] Unexpected response, retrying..." << std::endl;
            }
        }
    }
}




std::vector<char> udp_client::UDPClient::makePacket(const std::string& payload) {
    udp_packet::PacketHeader header{};
    header.seq = seq_;
    header.ack = 0; // No ACK for initial packet
    header.flags = udp_packet::Flags::DATA;
    header.len = payload.size();

    std::vector<char> buffer(sizeof(header) + header.len);
    memcpy(buffer.data(), &header, sizeof(header));
    memcpy(buffer.data() + sizeof(header), payload.data(), payload.size());

    return buffer;
}


bool udp_client::UDPClient::sendData(const std::string& payload) {
    auto packet = makePacket(payload);
    ssize_t sent = sendto(sock_, packet.data(), packet.size(), 0, (sockaddr*)&servaddr_, sizeof(servaddr_));
    if (sent < 0) {
        perror("sendto");
        return false;
    }
    return true;
}

udp_packet::Flags udp_client::UDPClient::receiveResponse() {
    char buffer[BUFFER_SIZE];
    ssize_t n = recvfrom(sock_, buffer, sizeof(buffer), 0, nullptr, nullptr);
    if (n < 0) {
        perror("recvfrom");
        return udp_packet::Flags::ERROR; // Indicate error
    }

    auto* header = reinterpret_cast<udp_packet::PacketHeader*>(buffer);
    return header->flags; // Return the flags from the received packet
}