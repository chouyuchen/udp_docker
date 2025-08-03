#pragma once
#include "packet.hpp"
#include <cstdint>
#include <string>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

namespace udp_client {

class UDPClient {
public:
    UDPClient(const std::string& host, uint16_t port);
    ~UDPClient();

    void run();

private:
    int sock_;
    sockaddr_in servaddr_;
    uint32_t seq_;


    bool sendData(const std::string& payload);

    udp_packet::Flags receiveResponse();

    std::vector<char> makePacket(const std::string& payload); // construct packet with header and payload
};

} // namespace udp_client