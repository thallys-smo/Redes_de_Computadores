#define TX 13    // Pino que ocorrerá a transmissão
#define BAUD_RATE 1  // Define baud rate  da comunicação
#define RTS 2    
#define CTS 3    
#define PAR // Define paridade par
#define B0_7

#include "Temporizador.h"

char volatile bit_index = 0;
char volatile dado_enviado;



// Calcula bit de paridade - Par ou impar
bool bitParidade(char dado){
  int  contador_1 = 0;
  for(char j = 0; j < 8; j++){
    if(bitRead(dado,j) == 1){
      contador_1++;
    }
  }
  
  #ifdef PAR
    if(contador_1%2==0){ // Número de 1s é par - mantém bit de paridade como 0
      return 0;
    }else{ // Número de 1s é ímpar - adiciona um 1 ao bit de paridade
      return 1; 
    }
  #else // Paridade ímpar
    if(contador_1%2==0){ // Número de 1s é par -adiciona 1 ao bit de paridade
      return 1
    }else{ // Número de 1s é ímpar - amantém bit de paridade como 0 
      return 0 
    }
  #endif

}


// Rotina de interrupcao do timer1
ISR(TIMER1_COMPA_vect){
  bool value = HIGH;
  Serial.println("Interrupcao - envio de bits");

  switch(bit_index){
    case 8: // Para index igual a 8, teremos o fim do byte (8 elementos) e devemos enviar o bit de paridade
      value = bitParidade(dado_enviado);
      Serial.print("Paridade = ");
      Serial.println(value);
      break;
    case 9: // stop bit
      value = HIGH;
      break;
    case 10:
      paraTemporizador();
      value = HIGH;
      break;
    default: // Envio dos bits de dados
      #ifdef B0_7 // Envia bits na ordem crescente - do menos significativo para o mais
            value = bitRead(dado_enviado,bit_index);
      #else
            value = bitRead(dado_enviado,(7 - bit_index));
      #endif
            break;
    }
  if bit_index!=10{
    digitalWrite(TX,value);
    Serial.println("Bit transmitido em TX: ");
    Serial.print(value);
  }  
  bit_index++;
}

// Função executada toda vez que o arduino for reiniciado
void setup(){

  //Desabilita interrupcoes enquanto roda as configurações do setup - evitar que ocorram transmissões antes de terminar as configurações iniciais
  noInterrupts();

  // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
  Serial.begin(9600);

  // Configuração da pinagem
  // PInos para o handshake
  pinMode(RTS,OUTPUT);
  pinMode(CTS,INPUT);
  // Pino usado para a transmissão
  pinMode(TX,OUTPUT);

  // Inicialização dos sinais de transmissão
  digitalWrite(RTS,LOW);
  digitalWrite(TX,HIGH);

  // Configura timer
  configuraTemporizador(BAUD_RATE);

  // Habilita interrupcoes
  interrupts();

}

// O loop() eh executado continuamente (como um while(true))
void loop ( ) {
  // Verifica se existe caractere para ser enviado em loop
  do{
    dado_enviado = Serial.read();
  }while(dado_enviado == -1);
  // Caracter recebido, manda RTS
  digitalWrite(RTS,HIGH);

  // Espera recebimento da confirmação do receptor - CTS em HIGH
  while(digitalRead(CTS) == LOW){
      Serial.println("Esperando retorno do repector - handshake");
  }
  Serial.println("CTS recebido");

  // Inicia transmissao
  digitalWrite(TX,LOW); // Start bit
  bit_index = 0;
  iniciaTemporizador();
  while(bit_index < 11); // Espera fim da transmissao

  // Indica que a transmissão foi finalizada - RTS para low
  digitalWrite(RTS,LOW);
  // Espera receptor confirmar finalização da comunicação (CTS LOW)
  while(digitalRead(CTS) == HIGH);
}
