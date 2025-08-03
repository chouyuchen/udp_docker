#include "udp_server.hpp"
#include <iostream>

#define SERVER_PORT 9000

int main(){
    try{
        udp_server::UDPServer server(SERVER_PORT);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}