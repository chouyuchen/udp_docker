# UDP Communication

This project demonstrates basic UDP communication between a client and a server using separate Docker.

## Quick Start

1. Start Docker containers  
   ```bash
   docker compose up
2. In a new terminal window, open the shell in the server container
    ```bash
    docker compose exec server bash
3. In another terminal window, open the shell in the client container
    ```bash
    docker compose exec client bash
4. First running the server inside the server container:
    ```bash
    cd build
    ./udp_server         # Run the main server application
    ./test_udp_server    # Run unit tests for the server
    ```
    If made any changes to the source code, recompile it by running:
    `cmake ..` and `make`
5. Then running the client inside the client container:
    ```bash
    cd build
    ./udp_client
    ./test_udp_client
    ```