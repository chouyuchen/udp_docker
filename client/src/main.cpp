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

    std::cout << "Hello world from client" << std::endl;
    while (true) {
        // send message to server:9000
        std::string msg = "cc " + std::to_string(seq);
        ssize_t sent = sendto(sock, msg.c_str(), msg.size(), 0, (sockaddr*)&servaddr, sizeof(servaddr));
        if (sent < 0) {
            perror("sendto");}
        else {
            std::cout << "[Client] Sent \"" << msg << "\"\n";
        }

        // wait for response from server
        char buffer[BUFFER_SIZE];
        ssize_t n = recvfrom(sock, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
        if (n < 0) {
            perror("recvfrom");
        } else {
            buffer[n] = '\0';
            std::cout << "[Client] Received \"" << buffer << "\"\n";
        }
        seq++;

        // 0.1 Hz
        std::this_thread::sleep_for(std::chrono::seconds(10));

    }
    return 0;
}