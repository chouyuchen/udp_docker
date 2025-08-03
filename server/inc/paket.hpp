#pragma once  
#include <cstdint>

namespace udp_packet{

enum class Flags : uint16_t {
    DATA = 0x1,
    ACK  = 0x2,
    NAK  = 0x4,
    ERROR = 0x8
};

#pragma pack(push,1) // Ensure the structure is packed without padding
struct PacketHeader {
    uint32_t seq; // 4 bytes , sequence number for payload
    uint32_t ack_num;  // 4 bytes, acknowledgment number for received packets
    uint16_t flags; // 2 bytes, define the type of packet
    uint16_t len;   // 2 bytes, payload length
};
#pragma pack(pop) // Restore the previous packing alignment

} // namespace udp_packet