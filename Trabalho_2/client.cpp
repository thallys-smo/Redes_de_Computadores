#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

// g++ -o client client.cpp -lpthread
// ./client

int createSocket();
void defineSocketAddr(struct sockaddr_in &server_addr, const std::string &ip, int port);
void connectToSocket(int socketFD, struct sockaddr_in &server_addr, int port); 
void recvData(int client_socketFD);
void sendData(int socket, const std::string &username);
std::string getUserInput(void);
void sendMessage(int socketFD, const std::string &message);

int main() {
    
    int socket = 0;
    struct sockaddr_in serv_addr;

    // Create socket file descriptor
    socket = createSocket();

    // Define the server address
    int server_port = 8080;
    defineSocketAddr(serv_addr, "127.0.0.1", server_port);

    // Connect to the server
    connectToSocket(socket, serv_addr, server_port);

    std::cout << std::endl << " --------  GROUP CHAT: Aplicação com Sockets TCP  -----------" << std::endl << std::endl << std::endl ;

    std::string username;
    std::cout << "Informe seu nome de usuário: ";
    std::getline(std::cin, username);
    sendMessage(socket, username);


    std::cout << std::endl << "Bem vindo a nossa aplicação " << username << "!" << std::endl;
    std::cout << "Envie e receba mensagens livremente para seus amigos!" << std::endl <<  std::endl << "Caso deseje visualizar possíveis comandos, basta mandar help" << std::endl << std::endl;


    // Chat loop
    //Sending data -> Deve rodar de forma paralela já que o recebimento do input pelo usuário bloqueia a execução
    std::thread sendThread(sendData, socket, username);
    std::thread recvThread(recvData, socket);

    sendThread.join();
    recvThread.join();

    // Close the socket
    close(socket);
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


void connectToSocket(int socketFD, struct sockaddr_in &server_addr, int port) {
    if (connect(socketFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Conexão estabelecida com servidor na porta " << port << std::endl;

}

void sendData(int socket, const std::string &username){
    while(true){
        // Get message from client user
        std::string message = getUserInput();

        // std::string completeMsg = username + ": " + message;

        if(message.back() == '\n'){
            message.pop_back();
        }

        // Send message to server
        sendMessage(socket, message);
    }
}

std::string getUserInput(void) {
    std::string clientMsg;
    std::getline(std::cin, clientMsg);

    return clientMsg;
}

void sendMessage(int socketFD, const std::string &message) {
    send(socketFD, message.c_str(), message.length(), 0);
}


void recvData(int client_socketFD) {
    char buffer[1024] = {0};
    while(true){
        memset(buffer, 0, sizeof(buffer));
        int recvData_len = recv(client_socketFD, buffer, sizeof(buffer), 0); 
        if (recvData_len > 0) {
            std::cout << std::endl << buffer << std::endl;
        }
        else if (recvData_len <= 0) {
            std::cout << "ERRO: Perda de conexão com o servidor" << std::endl;
            break;
        }
    }
    return;
}