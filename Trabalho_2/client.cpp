// client.cpp
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    // Define the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    // Chat loop
    while (true) {
        // Get message from client user
        std::cout << "Client: ";
        std::string message;
        std::getline(std::cin, message);
        
        // Send message to server
        send(sock, message.c_str(), message.length(), 0);

        // Clear buffer
        memset(buffer, 0, sizeof(buffer));
        
        // Read reply from server
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) {
            std::cout << "Server disconnected or error occurred" << std::endl;
            break;
        }
        std::cout << "Server: " << buffer << std::endl;
    }

    // Close the socket
    close(sock);
    return 0;
}
