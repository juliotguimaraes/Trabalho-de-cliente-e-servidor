/*
Header dos métodos executados pelo servidor.
*/

#ifndef SERVIDOR_H
#define SERVIDOR_H

#define BUFFERSIZE 576 //Tamanho maximo da mensagem
//#define DOOR 3333 //Porta para testar
#define MAX_CLIENTES 100 //Numero maximo de clientes que podem se conectar a este servidor ao mesmo tempo
#define AFFAMILY AF_INET //Default: ipv4. Caso deseja ipv6, colocar AF_INET6

typedef struct Usuario{

	char apelido[50];
	char ip[15];
	char porta[6];
	int status;
}Usuario;

Usuario *user; //Vetor de usuários do servidor

int inicia_socket_tcp(); //Inicia o socket no protocolo TCP
void liga_endereco_ao_socket(int porta, int skt); //Liga o endereço ao socket
int aceita_conexao(int skt_servidor, int *portaCliente); //Aceita conexão com o cliente
void zera_endereco(); //Zera os bytes do endereço do servidor
void le_do_socket(int skt, char * buffer); //Lê mensagem do socket
void escreve_no_socket(int skt, char * mensagem); //Escreve mensagem no socket
void verifica_numero_argumentos(int argc); //Verifica o numero de argumentos na chamada da main
void mensagem_erro(char * mensagem); //Envia mensagem de erro
char * retorna_buffer(int tamanho); //Retorna um buffer do tamanho do parâmetro
void imprime_mensagem(char * buffer); //Imprime mensagem do buffer na tela



#endif
