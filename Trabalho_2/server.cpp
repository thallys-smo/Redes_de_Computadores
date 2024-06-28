#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//g++ -o server server.cpp
// ./server

int createSocket();
void defineSocketAddr(struct sockaddr_in &server_addr, const std::string &ip, int port);
void bindSocket(int socketFD, struct sockaddr_in &socket_addr, int server_port);
void socket_listenToConections(int socketFD, int maxConections);
int socket_acceptConnection(int socketFD, struct sockaddr_in &client_addr, socklen_t &client_addrlen);
int receiveData(int client_socketFD, char *buffer);
std::string getUserInput(void);
void sendData(int socketFD, const std::string &message);

int main() {
    int server_socketFD, new_socket;
    struct sockaddr_in server_addr;
    
    // Cria o socket
    server_socketFD = createSocket();

    int server_port = 8080;
    std::string server_IP = ""; // Se quiser usar o server ouça qualquer ip deixar vazio ""
    if (server_IP == "localHost") {
        server_IP = "127.0.0.1";
    }

    // Configura o endereço do socket
    defineSocketAddr(server_addr, server_IP, server_port);
    socklen_t server_addrlen = sizeof(server_addr);

    // Bind o socket no endereço definido
    bindSocket(server_socketFD, server_addr, server_port);

    // Ouvindo conexões (limite de 10 conexões)
    int maxConections = 10;
    socket_listenToConections(server_socketFD, maxConections);

    // Aceita novas conexões
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof(client_addr);
    new_socket = socket_acceptConnection(server_socketFD, client_addr, client_addrlen);


    char buffer[1024] = {0};

    // Loop de comunicação
    while (true) {
        int recvData_len = receiveData(new_socket, buffer);
        if (recvData_len <= 0) {
            break;
        }
        std::string serverMsg;
        serverMsg = getUserInput();      
        sendData(new_socket, serverMsg);
        
    }

    // Fecha os sockets
    close(new_socket);
    close(server_socketFD);
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
    if(ip==""){
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }else{
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
}

void bindSocket(int socketFD, struct sockaddr_in &socket_addr, int server_port) {
    if (bind(socketFD, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        perror("Bind do servidor no endereço fornecido falhou");
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    else{
        std::cout << "Bind estabelecidao com port " <<  server_port << std::endl;    }
}

void socket_listenToConections(int socketFD, int maxConections) {
    if (listen(socketFD, maxConections) < 0) {
        perror("Listen");
        close(socketFD);
        exit(EXIT_FAILURE);
    }
}

int socket_acceptConnection(int socketFD, struct sockaddr_in &client_addr, socklen_t &client_addrlen) {
    int client_socket;

    if ((client_socket = accept(socketFD, (struct sockaddr *)&client_addr, &client_addrlen)) < 0) {
        perror("ERRO: Falha ao aceitar nova conexão");
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

int receiveData(int client_socketFD, char *buffer) {
    memset(buffer, 0, sizeof(buffer));
    int recvData_len = recv(client_socketFD, buffer, sizeof(buffer), 0); 
    std::cout << "Cliente: " << buffer << std::endl;
    return recvData_len;
}

std::string getUserInput(void){
    std::string serverMsg;
    std::cout << "Servidor: ";
    std::getline(std::cin, serverMsg);

    return serverMsg;
}

void sendData(int socketFD, const std::string &message) {
    send(socketFD, message.c_str(), message.length(), 0);
}
