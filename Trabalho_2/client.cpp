#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// g++ -o client client.cpp
// ./client

int createSocket();
void defineSocketAddr(struct sockaddr_in &server_addr, const std::string &ip, int port);
void bindSocket(int socketFD, struct sockaddr_in &socket_addr, int server_port);
void connectToSocket(int socketFD, struct sockaddr_in &server_addr); 
int receiveData(int client_socketFD, char *buffer);
std::string getUserInput(void);
void sendData(int socketFD, const std::string &message);

int main() {
    int clientSocket = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket file descriptor
    clientSocket = createSocket();

    // Define the server address
    int server_port = 8080;
    defineSocketAddr(serv_addr, "127.0.0.1", server_port);

    // Connect to the server
    connectToSocket(clientSocket, serv_addr);

    // Chat loop
    while (true) {
        // Get message from client user
        std::string message = getUserInput();
        
        // Send message to server
        sendData(clientSocket, message);

        // Read reply from server
        int valread = receiveData(clientSocket, buffer);
        if (valread <= 0) {
            std::cout << "Server disconnected or error occurred" << std::endl;
            break;
        }
    }

    // Close the socket
    close(clientSocket);
    return 0;
}

int createSocket() {
    int server_socketFD;
    
    // Cria o socket file descriptor 
    if ((server_socketFD = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // SOCK_STREAM -> IPv4
        perror("Criação do socket falhou");
        exit(EXIT_FAILURE);
    }

    return server_socketFD;
}

void defineSocketAddr(struct sockaddr_in &server_addr, const std::string &ip, int port) {
    // Define o endereço do servidor
    server_addr.sin_family = AF_INET; // Especifica protocolo IPv4
    server_addr.sin_port = htons(port); // Converte o número da porta fornecida para o formato aceito pela rede

    // Se ip estiver definido como localHost o servidor irá ser capaz de ouvir em qualquer IP disponível
    if (ip == "") {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
}

void bindSocket(int socketFD, struct sockaddr_in &socket_addr, int server_port) {
    if (bind(socketFD, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        perror("Bind do servidor no endereço fornecido falhou");
        close(socketFD);
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Bind estabelecidao com port " <<  server_port << std::endl;
    }
}

void connectToSocket(int socketFD, struct sockaddr_in &server_addr) {
    if (connect(socketFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int receiveData(int client_socketFD, char *buffer) {
    memset(buffer, 0, sizeof(buffer));
    int recvData_len = recv(client_socketFD, buffer, sizeof(buffer), 0); 
    if (recvData_len > 0) {
        std::cout << "Servidor: " << buffer << std::endl;
    }
    return recvData_len;
}

std::string getUserInput(void) {
    std::string clientMsg;
    std::cout << "Cliente: ";
    std::getline(std::cin, clientMsg);

    return clientMsg;
}

void sendData(int socketFD, const std::string &message) {
    send(socketFD, message.c_str(), message.length(), 0);
}
