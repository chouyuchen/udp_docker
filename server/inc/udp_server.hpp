#pragma once
#include "paket.hpp"
#include <cstdint>
#include <string>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

namespace udp_server {

class UDPServer {
public:
    UDPServer(const uint16_t port);
    ~UDPServer();
    void run();

private:
    int sock_;
    uint32_t expected_seq_;

    void handlePacket(const char* buffer, ssize_t n,
                      const sockaddr_in& client, socklen_t clientlen);
    void sendResponse(uint32_t ack_num, udp_packet::Flags f,
                     const sockaddr_in& client, socklen_t clientlen);

};

}  // namespace udp_server