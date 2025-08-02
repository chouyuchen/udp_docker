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

    
    uint64_t seq = 0; // counter for sequence 

    std::cout << "Hello world from server" << std::endl;

    while (true) {
        // receive data from any client message
        char buffer[BUFFER_SIZE];
        sockaddr_in clientaddr{};
        socklen_t clientlen = sizeof(clientaddr);
        ssize_t n = recvfrom(sock, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientaddr, &clientlen);
        if (n < 0) { 
            perror("recvfrom");
            continue;
        }
        buffer[n] = '\0';
        std::cout << "[Server] Received \"" << buffer
                  << "\" from " << inet_ntoa(clientaddr.sin_addr)
                  << ":" << ntohs(clientaddr.sin_port) << "\n";


        // send response back to client
        std::string reply = "ss " + std::to_string(seq++);
        ssize_t sent = sendto(sock, reply.c_str(), reply.size(), 0, (sockaddr*)&clientaddr, clientlen);
        if (sent < 0) {
            perror("sendto");
            continue;
        
        }

        // 0.1 Hz
        std::this_thread::sleep_for(std::chrono::seconds(10)); 

    }
    return 0;
}