# Redes de Computadores - SSC0142 (2024)

## Chat em grupo - TRABALHO 2

### Alunos 
- Matheus Della Rocca Martins       - 12549731
- Thallys Simões de Morais Oliveira - 11819827

### Vídeo de Demonstração:


https://github.com/thallys-smo/Redes_de_Computadores/assets/72418094/9606a23c-e33d-4396-9e34-f1079833d308

### Proposta

Este projeto implementa um chat em grupo utilizando sockets TCP e endereçamento com IPv4. Neste sentido, visa-se desenvolver uma aplicação que permite a troca de mensagens, gerenciadas por um servidor, entre diversos usuários conectados em rede em uma mesma máquina. 

Dessa forma, o servidor permite que múltiplos clientes se conectem simultaneamente e possibilita que cada cliente envie mensagens broadcast (mensagens destinadas aos demais clientes conectados) ou mensagens diretas (mensagens destinadas para um cliente específico). Além disso, foram implementadas algumas funcionalidades simples para melhorar a experiência dos usuários. 


### Compilação e Execução

**Requisitos**:

Os requisitos deste projeto normalmente já vem instalados por padrão no linux. Entretanto, caso tenha problemas seguem as devidas instruções: 

Para compilar e executar este projeto, são necessárias as seguintes bibliotecas:

- **gcc**: Compilador GNU para C e C++

    ```sh
    sudo apt update
    sudo apt install gcc
    ```

- **libstdc++**: Biblioteca padrão do C++
  - Instalação adicional não necessária no Ubuntu/Debian, incluída com o GCC.

- **pthread**: Biblioteca para suporte a threads POSIX.

    ```sh
    sudo apt update
    sudo apt install libpthread-stubs0-dev
    ```

**Compilando o Servidor**:

- Navegue até o diretório onde o código do servidor está localizado.
- Compile o servidor com o seguinte comando:

    ```sh
    gcc -o server server.cpp -lstdc++ -lpthread
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
    gcc -o client client.cpp -lstdc++ -lpthread
    ```

**Executando o Cliente**:

- Após a compilação, execute o cliente com:

    ```sh
    ./client
    ```

Certifique-se de iniciar o servidor antes de executar os clientes. Cada cliente deve se conectar ao servidor especificando o IP e a porta correspondentes.

### Funcionalidades

- **Mensagens Broadcast**: Permite enviar mensagens para todos os clientes conectados ao servidor.
- **Mensagens Diretas**: Permite enviar mensagens diretamente para um cliente específico. Para enviar uma mensagem direta, utilize o comando `dm nomeDoCliente: mensagem` no terminal.
- **Mensagens do servidor**: Permite que sejam enviadas mensagens/avisos, destinadas para todos os clientes, através do servidor.
  
**Comandos Especiais**
- **Comando de ajuda**: Fornece um comando para exibir a lista de comandos disponíveis. Para utiliza-lo, envie `help` através dos clientes.
- **Lista de Clientes**: Possibilidade de listar todos os clientes conectados ao servidor. Para utiliza-lo, envie `client list` através dos clientes.


