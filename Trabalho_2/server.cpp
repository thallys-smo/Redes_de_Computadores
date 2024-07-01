#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>

#include <vector>

// g++ -o server server.cpp -lpthread
// ./server

// Variáveis para armazenar informações do cliente
struct ClientInfo_Struct {
    int client_Socket;
    sockaddr_in client_Address;
    int client_ID;
    std::string client_Name;
};
std::vector<ClientInfo_Struct> clients_list;
int clientID_counter = 0;

// Declaração das funções
int createSocket();
void defineSocketAddr(struct sockaddr_in &server_addr, const std::string &ip, int port);
void bindSocket(int socketFD, struct sockaddr_in &socket_addr, int server_port);
void socket_listenToConections(int socketFD, int maxConections);
void server_recvConections(int server_socketFD);
void socket_dealWithNewConnections(int server_socketFD, int client_ID, const std::string client_name);
int socket_acceptConnection(int socketFD, struct sockaddr_in &client_addr, socklen_t &client_addrlen);
void recvData(int client_socketFD, int client_ID, const std::string origin_clientName);
void sendDirectMsg(const std::string &message, const std::string clientName);
void broadcastClientMsg(const std::string &message, int clientID);
void sendServerMsg(int socket);
std::string getUserInput(void);
void sendMessage(int socketFD, const std::string &message);


int main() {
    int server_socketFD;
    struct sockaddr_in server_addr;
    
    // Cria o socket do servidor
    server_socketFD = createSocket();

    // Configurações de porta e ip do socket
    int server_port = 8080;
    std::string server_IP = ""; // Se quiser usar o server ouça qualquer ip deixar vazio ""
    if (server_IP == "localHost") {
        server_IP = "127.0.0.1";
    }

    // Configura o endereço do socket
    defineSocketAddr(server_addr, server_IP, server_port);
    socklen_t server_addrlen = sizeof(server_addr);

    // Linka o socket no endereço definido
    bindSocket(server_socketFD, server_addr, server_port);

    // Ouvindo conexões (limite de 15 conexões)
    int maxConections = 15;
    socket_listenToConections(server_socketFD, maxConections);

    // Trata conexões recebidas
    server_recvConections(server_socketFD); 

    // Encerra o socket do servidor
    close(server_socketFD);

    return 0;
}



int createSocket() {
    int server_socketFD;
    
    // Cria o socket file descriptor para os protocolos IPv4 e TCP
    if ((server_socketFD = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        std::cout << "ERRO: Criação do socket falhou" << std::endl;
        exit(EXIT_FAILURE);
    }

    return server_socketFD;
}

void defineSocketAddr(struct sockaddr_in &server_addr, const std::string &ip, int port) {
    // Define o endereço do servidor
    server_addr.sin_family = AF_INET; // Especifica protocolo IPv4
    server_addr.sin_port = htons(port); // Converte o número da porta fornecida para o formato aceito pela rede

    // Se ip estiver definido como "" o servidor irá ser capaz de ouvir em qualquer IP disponível
    if(ip==""){
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }else{
        server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
}

void bindSocket(int socketFD, struct sockaddr_in &socket_addr, int server_port) {
    // Linka o socket com o endereço fornecido
    if (bind(socketFD, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        std::cout << "Bind do servidor no endereço fornecido falhou" << std::endl;
        close(socketFD);
        exit(EXIT_FAILURE);
    }
    else{
        std::cout << "Bind estabelecidao na porta " <<  server_port << std::endl;    }
}

void socket_listenToConections(int socketFD, int maxConections) {
    // Mantém o socket ouvindo novas conexões até que se alcance o limite definido
    if (listen(socketFD, maxConections) < 0) {
        std::cout << "ERRO: Falha ao escutar novas conexões" << std::endl;
        close(socketFD);
        exit(EXIT_FAILURE);
    }
}

void server_recvConections(int server_socketFD){
    while(true){
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        // Aceitação de uma nova conexão
        int client_socket = socket_acceptConnection(server_socketFD, client_addr, client_addrlen);

        char name_input[1024] = {0};
        recv(client_socket, name_input, sizeof(name_input), 0);

        std::string client_name(name_input);

        // Conexão estabelecida
        std::cout << std::endl << "Novo cliente conectado: " << client_name << std::endl;

        int client_ID = clientID_counter;

        // Adiciona novo cliente à lista de clientes
        clients_list.push_back({client_socket, client_addr, client_ID, client_name});
        clientID_counter++;

        // Lida com os clientes de forma paralela
        std::thread thread_dealWithNewConnections(socket_dealWithNewConnections, client_socket, client_ID, client_name);
        thread_dealWithNewConnections.detach(); 
    }
    return;
}

int socket_acceptConnection(int socketFD, struct sockaddr_in &client_addr, socklen_t &client_addrlen) {
    int client_socket;

    if ((client_socket = accept(socketFD, (struct sockaddr *)&client_addr, &client_addrlen)) < 0) {
        std::cout << "ERRO: Falha ao aceitar nova conexão" << std::endl;        
        close(socketFD);
        exit(EXIT_FAILURE);
    }

    return client_socket;
}

void socket_dealWithNewConnections(int client_socketFD, int client_ID, const std::string client_name){
    // Loop de comunicação
    std::thread recvThread(recvData, client_socketFD, client_ID, client_name);
    std::thread sendServerMsgThread(sendServerMsg, client_socketFD);
    recvThread.join();
    sendServerMsgThread.detach(); 

    // Fecha os sockets
    close(client_socketFD);

    // Tirar client da lista
    for (auto it = clients_list.begin(); it != clients_list.end(); ++it) {
        if (it->client_ID == client_ID) {
            clients_list.erase(it);
            break;  // Importante: sair do loop após a remoção
        }
    }

    return;
}

void recvData(int client_socketFD, int client_ID, const std::string origin_clientName) {
    char buffer[1024] = {0};
    while(true){
        memset(buffer, 0, sizeof(buffer));
        int recvData_len = recv(client_socketFD, buffer, sizeof(buffer), 0); 
        std::string receivedMessage(buffer);
        if (recvData_len > 0) {
            std::string headerMsg = origin_clientName + ": "; 

            // Envio de uma mensagem direta para outro cliente
            if (receivedMessage.find("dm ") == 0) {      
                for (const auto &client : clients_list) {
                    std::string directMsgSign = "dm " + client.client_Name + ":";
                    if(receivedMessage.find(directMsgSign)==0){
                        std::string directMsg = receivedMessage.substr(directMsgSign.length(), receivedMessage.length()-1);
                        // Tirar espaço do início da mensagem caso exista
                        if(directMsg[0] == ' '){
                            directMsg = directMsg.substr(1, directMsg.length()-1);
                        }
                        directMsg = "(DM) " + headerMsg + directMsg;
                        std::cout << std::endl << "Mandando mensagem direta para " << client.client_Name << " de " << origin_clientName << std::endl; 
                        sendDirectMsg(directMsg, client.client_Name);
                    }
                }
            // Servidor retorna lista de comandos para o usuário
            }else if(receivedMessage=="help"){
                std::string helpMsg = "Possíveis comandos:\n"
                          "- *dm nomeDoCliente:* -> Envia mensagem direta para outro cliente.\n"
                          "- *client list* -> Retorna uma lista de todos os clientes conectados.\n";
                sendDirectMsg(helpMsg, origin_clientName);
            }else if(receivedMessage=="client list"){
                std::string listMsg = "Lista de clientes conectados:\n";
                for (const auto &client : clients_list) {
                    listMsg += "- " + client.client_Name + "\n";
                }
                sendDirectMsg(listMsg, origin_clientName);

            }else{
                // Envio da mensagem para todos os outros clientes (broadcast mode)
                std::string broadcastMsg = "(broadcast) " + headerMsg + buffer; 
                broadcastClientMsg(broadcastMsg, client_ID);
            }
        }
        else if (recvData_len <= 0) {
            std::cout << std::endl << "ERRO: Perda de conexão com o client -> " << origin_clientName << std::endl;
            break;
        }
    }
    return;
}


void sendDirectMsg(const std::string &message, const std::string targetClient){
    for (const auto &client : clients_list) {
        if (client.client_Name == targetClient) {
            sendMessage(client.client_Socket, message);
        }
    }
    return;
}


void broadcastClientMsg(const std::string &message, int clientID){
    // Percorre a lista de clientes e manda a mensagem para todos que tiverem um ID diferente do cliente
    for (const auto &client : clients_list) {
        if (client.client_ID != clientID) {
            sendMessage(client.client_Socket, message);
        }
    }
    return;
}



void sendServerMsg(int socket){
    std::string name = "Servidor";
    while(true){
        // Recebe mensagem do gerenciador do servidor
        std::string message = getUserInput();

        std::string completeMsg = name + ": " + message;
        // Manda mensagem do servidor para todos os clientes
        for (const auto &client : clients_list) {
            sendMessage(client.client_Socket, completeMsg);
        }
    }

    return;
}


std::string getUserInput(void) {
    std::string clientMsg;
    std::getline(std::cin, clientMsg);

    return clientMsg;
}


void sendMessage(int socketFD, const std::string &message) {
    send(socketFD, message.c_str(), message.length(), 0);
}
