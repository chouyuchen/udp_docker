#include <iostream>
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <vector>

#define SERVER_PORT 9000
#define BUFFER_SIZE 1024

enum Flags : uint16_t {
    DATA = 0x1,
    ACK  = 0x2,
    NAK  = 0x4
};

#pragma pack(push,1)
struct PacketHeader {
    uint32_t seq;
    uint32_t ack;
    uint16_t flags;
    uint16_t len;
};
#pragma pack(pop)


int main() {
    // set up udp socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    // resolve server address
    addrinfo hints{}, *res;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP
    if(getaddrinfo("server", std::to_string(SERVER_PORT).c_str(), &hints, &res) != 0) {
        perror("getaddrinfo");
        close(sock);
        return 1;
    }
    sockaddr_in servaddr = *(sockaddr_in*)res->ai_addr;
    freeaddrinfo(res); // free addrinfo structure

    
    uint64_t seq = 0;

    std::cout << "Client up and running" << std::endl;
    while (true) {
        // send message to server:9000
        std::string payload = "msg#" + std::to_string(seq);
        PacketHeader header{};
        header.seq = seq;
        header.ack = 0;
        header.flags = Flags::DATA;
        header.len = payload.size();

        std::vector<char> buffer(sizeof(header) + header.len);
        std::memcpy(buffer.data(), &header, sizeof(header));
        std::memcpy(buffer.data() + sizeof(header), payload.data(), payload.size());

        bool sent_done = false;
        while(!sent_done) {
            ssize_t sent = sendto(sock, buffer.data(), buffer.size(), 0, (sockaddr*)&servaddr, sizeof(servaddr));
            if (sent < 0) {
                perror("sendto");
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // retry after a short delay
            } 
            char buffer[BUFFER_SIZE];
            ssize_t n = recvfrom(sock, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
            auto *header = (PacketHeader*)buffer;
            if(header->flags & NAK){
                std::cout << "[Client] Received NAK for seq " << header->ack << ", resending...\n";
            } else if(header->flags & ACK) {
                std::cout << "[Client] Received ACK for seq " << header->ack << "\n";
                sent_done = true; // exit loop on successful ACK
            } else {
                std::cout << "[Client] Received unexpected packet\n";
            }
        }
        seq++;
        
        // 0.1 Hz
        std::this_thread::sleep_for(std::chrono::seconds(10));

    }
    return 0;
}