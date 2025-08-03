#include "udp_client.hpp"
#include <iostream>

int main() {
    try {
        udp_client::UDPClient client("server", 9000);
        client.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
    return 0;
}