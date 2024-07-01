# Redes de Computadores - SSC0142 (2024)

## Chat em grupo - TRABALHO 2

### Resumo

Este projeto implementa um chat em grupo utilizando sockets IPv4 com o protocolo TCP. O servidor permite múltiplos clientes conectarem-se simultaneamente e possibilita que cada cliente envie mensagens broadcast (envia para os demais clientes conectados) ou mensagens diretas (envia para um cliente específico).

### Funcionalidades

- **Mensagens Broadcast**: Permite enviar mensagens para todos os clientes conectados ao servidor.
- **Mensagens Diretas**: Permite enviar mensagens diretamente para um cliente específico.
- **Comandos de Ajuda (help)**: Fornece um comando para exibir a lista de comandos disponíveis.
- **Lista de Clientes (client list)**: Possibilidade de listar todos os clientes conectados ao servidor.

### Compilação e Execução

**Compilando o Servidor**:

- Navegue até o diretório onde o código do servidor está localizado.
- Compile o servidor com o seguinte comando:

    ```sh
    g++ -o server server.cpp -lpthread
    ```

**Executando o Servidor**:

- Após a compilação, execute o servidor com:

    ```sh
    ./server
    ```

**Compilando o Cliente**:

- Navegue até o diretório onde o código do cliente está localizado.
- Compile o cliente com o seguinte comando:

    ```sh
    g++ -o client client.cpp -lpthread
    ```

**Executando o Cliente**:

- Após a compilação, execute o cliente com:

    ```sh
    ./client
    ```

Certifique-se de iniciar o servidor antes de executar os clientes. Cada cliente deve se conectar ao servidor especificando o IP e a porta correspondentes.

### Alunos 
- Matheus Della Rocca Martins       - 12549731
- Thallys Simões de Morais Oliveira - 11819827
