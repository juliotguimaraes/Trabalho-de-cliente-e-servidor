/*
Header dos metodos utilizador pelo cliente.c

*/

#ifndef CLIENTE_H
#define CLIENTE_H

#define BUFFERSIZE 576 // Tamanho maximo da mensagem.
#define AFFAMILY AF_INET //Default: ipv4. Caso deseja ipv6, colocar AF_INET6


int inicia_socket_tcp(); //Inicia o socket
void configura_servidor(int porta, char * nome_servidor);//Configura o servidor pelos parâmetros passados
void conecta_ao_servidor(int skt_cliente); //Conecta o socket cliente
void le_do_socket(int skt, char * buffer); //Lê mensagem do socket
void escreve_no_socket(int skt, char * mensagem); //Escreve mensagem do socket
void zera_endereco(); //Sera os bytes do enedereço 
void verifica_numero_argumentos(int argc); //Verifica número de argumentos
void mensagem_erro(char * mensagem); //Envia mensagem de erro
char * retorna_buffer(int tamanho); //Retornar buffer como o tamanho do parâmetro
FILE * open_file_read(char *file_name); //Abre arquivo para ser lido

//Servidor-Cliente UDP (Para que os usuários conversem entre si)
void servidorUDP(int porta);
void clienteUDP(char *ip, int porta);

#endif
