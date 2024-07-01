#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

// Comandos de compilação
// g++ -o client client.cpp -lpthread
// ./client

// Declaração das funções
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

    // Cria o socket file descriptor
    socket = createSocket();

    // Define o endereço do socket do servidor
    int server_port = 8080;
    defineSocketAddr(serv_addr, "127.0.0.1", server_port);

    // Conecta com o servidor
    connectToSocket(socket, serv_addr, server_port);

    std::cout << std::endl << " --------  Chat em grupo: Aplicação com Sockets TCP  -----------" << std::endl << std::endl << std::endl ;

    std::string username;
    std::cout << "Informe seu nome de usuário: ";
    std::getline(std::cin, username);
    // Envia nome de usuário para cadastro no servidor
    sendMessage(socket, username);


    std::cout << std::endl << "Bem vindo a nossa aplicação " << username << "!" << std::endl;
    std::cout << "Envie e receba mensagens livremente para seus amigos!" << std::endl <<  std::endl << "Caso deseje visualizar possíveis comandos, basta mandar help" << std::endl << std::endl;


    // Chat loop
    // Envio de dados deve rodar de forma paralela já que o recebimento do input pelo usuário bloqueia a execução da thread
    std::thread sendThread(sendData, socket, username);
    std::thread recvThread(recvData, socket);
    sendThread.join();
    recvThread.join();

    //  Fecha o socket criado
    close(socket);
    return 0;
}


int createSocket() {
    int server_socketFD;
    // Cria o socket file descriptor para os protocolos IPv4 e TCP
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
    // Se ip estiver definido como "" o servidor irá ser capaz de ouvir em qualquer IP disponível
    if (ip == "") {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
}


void connectToSocket(int socketFD, struct sockaddr_in &server_addr, int port) {
    // Conecta com o socket do servidor
    if (connect(socketFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Conexão estabelecida com servidor na porta " << port << std::endl;

}

void sendData(int socket, const std::string &username){
    while(true){
        // Recebe o input do usuário
        std::string message = getUserInput();

        // Remove \n do final da mensagem, caso exista
        if(message.back() == '\n'){
            message.pop_back();
        }

        // Repassa mensagem para o servidor
        sendMessage(socket, message);
    }
}

std::string getUserInput(void) {
    std::string clientMsg;
    std::getline(std::cin, clientMsg);

    return clientMsg;
}

void sendMessage(int socketFD, const std::string &message) {
    // Envia mensagem para o servidor 
    send(socketFD, message.c_str(), message.length(), 0);
}


void recvData(int client_socketFD) {
    char buffer[1024] = {0};
    while(true){
        memset(buffer, 0, sizeof(buffer));
        // Recebe dados do servidor e printa para o usuário
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