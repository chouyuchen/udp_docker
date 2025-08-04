#include <gtest/gtest.h>
#include "udp_client.hpp"
#include "paket.hpp"
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>



TEST(DUMMY_TEST, BasicAssertions) {
    // This is a dummy test to ensure the test framework is set up correctly
    EXPECT_EQ(sizeof(udp_packet::PacketHeader), 10u);
    EXPECT_EQ(sizeof(udp_packet::Flags), 2u);
}

