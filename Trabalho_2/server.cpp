// server.cpp
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Accept a connection from a client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Chat loop
    while (true) {
        // Clear buffer
        memset(buffer, 0, sizeof(buffer));
        
        // Read message from client
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "Client disconnected or error occurred" << std::endl;
            break;
        }
        std::cout << "Client: " << buffer << std::endl;
        
        // Get reply message from server user
        std::cout << "Server: ";
        std::string reply;
        std::getline(std::cin, reply);
        
        // Send reply to client
        send(new_socket, reply.c_str(), reply.length(), 0);
    }

    // Close the socket
    close(new_socket);
    close(server_fd);
    return 0;
}
