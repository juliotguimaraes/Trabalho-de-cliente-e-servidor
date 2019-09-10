/*
Julio Cesar Tadeu Guimaraes
2012049669
Ciencia da computação
Redes de computadores

**Implementação da parte cliente de um sistema cliente-servidor

*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include "cliente.h"
#include <poll.h>

//#define DEBUG

/*
Endereço do socket do servidor
*/
struct sockaddr_in endereco_servidor;

/*
Main do cliente.
init_c <nome_servidor ou IP> <porta>

<porta> = porta a se conectar o socket
<nome_servidor> = nome do servidor
*/
int main(int argc, char * argv[]){

	//Descrição do socket
	int	skt_servidor, porta;

	#ifdef DEBUG
	printf("Argumentos: %s %s %s\n", argv[1], argv[2]);
	#endif
	porta = atoi(argv[2]);

		printf("Porta: %d\n", porta);

	//Mede tempo
/*	struct timeval t1, t2;
	double tempo_decorrido;
*/
	char *portaCliente = (char *) malloc(sizeof(char) * 6);

	//Buffer para enviar as mensagens
	char * buffer = retorna_buffer(BUFFERSIZE+1);

	//Pega a descrição do socket
	skt_servidor = inicia_socket_tcp();

	//Configura o servidor dado a PORTA e o NOME
	configura_servidor(porta, argv[1]);

	//Conecta
	conecta_ao_servidor(skt_servidor);

	//Zera o buffer
	bzero(buffer, BUFFERSIZE);

	
	do{
		escreve_no_socket(skt_servidor, "READY");
		bzero(buffer, BUFFERSIZE);
		le_do_socket(skt_servidor, buffer);
		printf("%s\n\n", buffer);
	} while (strcmp(buffer, "READYACK"));

	do{ //RECEBE A PORTA QUE O USUARIO ESTÁ CONVERSANDO COM O SERVIDOR
		bzero(buffer, BUFFERSIZE);
		le_do_socket(skt_servidor, buffer);
		printf("%s\n\n", buffer);
		strcpy(portaCliente, buffer);
		bzero(buffer, BUFFERSIZE);
		escreve_no_socket(skt_servidor, "OK");
		le_do_socket(skt_servidor, buffer);
		printf("%s\n\n", buffer);
	} while (strcmp(buffer, "PORTAACK") != 0);
	
	//Switch de opcoes de conexao com o servidor
	printf("Digite seu apelido: ");
	char *apelido = (char *) malloc(sizeof(char) * 50);
	scanf("%s", apelido);

	printf("Digite seu IP (formato: 150.164.20.2): ");
	char *ip = (char *) malloc(sizeof(char) * 15);
	scanf("%s", ip);

	int status = 0; //Caso o cliente esteja apenas se conectando, ou seja, não está nem disponível nem conversando.
	int op = -1;
	while(op != 4){
		printf("0 - Inscrição\n1 - Receber lista de usuários\n2 - Atualizar status\n3 - Receber IP de um usuário para se conectar\n 4 - Remover inscrição e desconectar\nOpção: ");
		scanf("%d", &op);
		printf("Aqui: %d\n\n", op);
		switch(op){
			//Esses valores de opções também serão usadas para que o servidor defina qual ação o cliente está querendo.
			case 0:
						do{
							bzero(buffer, BUFFERSIZE);
							strcpy(buffer, "0,");
							strcat(buffer, apelido);
							strcat(buffer, ",");
							strcat(buffer, ip);
							strcat(buffer, ",");
							strcat(buffer, portaCliente);
							strcat(buffer, "\0");
							escreve_no_socket(skt_servidor, buffer);
							bzero(buffer, BUFFERSIZE);
							le_do_socket(skt_servidor, buffer);
							printf("%s\n", buffer);
						}while (strcmp(buffer, "INSCACK")); //Resposta de confirmação do servidor
						printf("Inscrição realizada.\n");
					//Enviar para o servidor apelido e ip
					//Recebe confirmação 
				break;
			case 1:
					//escreve_no_socket(skt_servidor, "ACK");
					bzero(buffer, BUFFERSIZE);
					strcpy(buffer, "1\0");
					escreve_no_socket(skt_servidor, buffer);
					do{
						le_do_socket(skt_servidor, buffer);
						if(strcmp(buffer, "LISTACK") != 0){
							printf("%s\n", buffer);
							printf("\n");
							bzero(buffer, BUFFERSIZE);
							escreve_no_socket(skt_servidor, "OK");
						}
					}while (strcmp(buffer, "LISTACK")); //Resposta de confirmação do servidor

					//Enviar apenas a opção para receber a lista de usuários
					//Recebe lista de usuários
				break;
			case 2:
				//Enviar apelido e status(1 - disponivel (servidor UDP) ; 2 - conversando (cliente UDP)) a ser atualizado 
				printf("Digite seu novo status (1 para disponível e 2 para conversando): \n");
				scanf("%d", &status);
				printf("Status: %d\n", status);
				do{
					bzero(buffer, BUFFERSIZE);
					strcpy(buffer, "2,");
					strcat(buffer, apelido);
					strcat(buffer, ",");
					char teste[2];
					sprintf(teste, "%d", status);
					strcat(buffer, teste);
					strcat(buffer, "\0");
					printf("%s\n", buffer);
					escreve_no_socket(skt_servidor, buffer);
					bzero(buffer, BUFFERSIZE);
					le_do_socket(skt_servidor, buffer);
				}while (strcmp(buffer, "STATACK") != 0); //Resposta de confirmação do servidor
				//Recebe confirmação ou entra em modo de conversa UDP
				if(status == 2){ //Se status Conversando, o cliente abre uma conexão udp para esperar por mensagens de outros usuarios
					servidorUDP(atoi(portaCliente));
					printf("Cliente UDP aberto na porta %d\nPara sair da conversa digite (TCHAU)\n", atoi(portaCliente));
				}
				break;
			case 3:
				printf("Digite o apelido do usuario que deseja receber o ip:\n");
				char *aux = (char *) malloc(sizeof(char) * 50);
				char *ip_user = (char *) malloc(sizeof(char) * 15);
				char *porta = (char *) malloc(sizeof(char) * 6);
				char *ponteiro;
				scanf("%s", aux);
				do{
					strcpy(buffer, "3,");
					strcat(buffer, aux);
					strcat(buffer, "\0");
					escreve_no_socket(skt_servidor, buffer);
					bzero(buffer, BUFFERSIZE);
					le_do_socket(skt_servidor, buffer);
					if(strcmp(buffer, "RECACK") != 0){
						ponteiro = strtok(buffer, ",");
						strcpy(ip_user, ponteiro);
						ponteiro = strtok(NULL, ",");
						strcpy(porta, ponteiro);
						bzero(buffer, BUFFERSIZE);
						escreve_no_socket(skt_servidor, "OK");
					}
				}while (strcmp(buffer, "RECACK") == 0);
					printf("Conectando ao Usuario %s:%s\n Para sair da conversa digite (TCHAU)\n", ip_user, porta);

					clienteUDP(ip_user, atoi(porta));
					//Enviar apelido do usuário que deseja o ip
					//Recebe ip
					//Conecta UDP
					free(aux);
					free(ip_user);
					free(porta);
				break;
			case 4:
					do{
							bzero(buffer, BUFFERSIZE);
							strcpy(buffer, "4,");
							strcat(buffer, apelido);
							strcat(buffer, "\0");
							escreve_no_socket(skt_servidor, buffer);
							bzero(buffer, BUFFERSIZE);
							le_do_socket(skt_servidor, buffer);

						}while (strcmp(buffer, "DESACK") != 0); //Resposta de confirmação do servidor
						printf("Desconectando.\n");
				break;
			default:
					printf("Opcão invalida\n");
				break;
		}
	}

	zera_endereco();	
	free(buffer);
	free(portaCliente);
	free(ip);
	free(apelido);
	return 0;
}

/*
Inicia socket no protocolo TCP.
Caso não haja erro, retorna um número que representa o arquivo de descrição
do Socket aberto. Caso ocorre algum erro o retorna será um número negativo.
*/

int inicia_socket_tcp(){

	int skt;
	#ifdef DEBUG
		printf("Abrindo o socket...\n");
	#endif
	skt = socket(AFFAMILY, SOCK_STREAM, IPPROTO_TCP);

	if (skt < 0){
		mensagem_erro("Erro ao abrir o socket");
	}

	#ifdef DEBUG
		printf("Socket %d aberto...\n", skt);
	#endif

	return skt;
}

void configura_servidor(int porta, char * nome_servidor){

	struct hostent * servidor;
	servidor = gethostbyname(nome_servidor);
	#ifdef DEBUG
		printf("Procurando o servidor %s...\n", nome_servidor);
	#endif
	if(servidor == NULL){

		fprintf(stderr, "Erro, nao existe esse servidor\n");
		exit(0);
	}

	bzero((char *)&endereco_servidor, sizeof(endereco_servidor));
	endereco_servidor.sin_family = AFFAMILY;
	bcopy((char *)servidor->h_addr, (char *)&endereco_servidor.sin_addr.s_addr, servidor->h_length);
	endereco_servidor.sin_port = htons(porta);
	#ifdef DEBUG
		printf("Conectado a %s !\n", nome_servidor);
	#endif
}

void conecta_ao_servidor(int skt_servidor){
	if(connect(skt_servidor, &endereco_servidor, sizeof(endereco_servidor)) < 0){

		mensagem_erro("Erro ao conectar com o servidor...");
	}
}

void le_do_socket(int skt, char * buffer){
	#ifdef DEBUG
		printf("Lendo do socket %d...\n", skt);
	#endif
	if (read(skt, buffer, BUFFERSIZE-1) < 0){

		mensagem_erro("Erro ao ler do socket");
	}
}

void escreve_no_socket(int skt, char * mensagem){

	#ifdef DEBUG
		printf("Escrevendo no socket %d...\n", skt);
	#endif
	if (write(skt, mensagem, strlen(mensagem)) < 0){

		mensagem_erro("Erro ao escrever no socket");
	}
}

void zera_endereco(){

	#ifdef DEBUG
		printf("Zerando o endereco do socket...\n");
	#endif

	//Zera a região de memória do endereco do servidor
	bzero((char*) &endereco_servidor, sizeof(endereco_servidor));
}

void verifica_numero_argumentos(int argc){
	
	if (argc < 3){

		fprintf(stderr, "Argumentos insuficientes.\nUso do metodo:init_c <porta> <nome_servidor> <nome_arquivo>\n");
		exit(1);
	}
}

void mensagem_erro(char * mensagem){

	perror(mensagem);
	exit(1);
}

char * retorna_buffer(int tamanho){

	char * buffer;
	buffer = malloc(tamanho*sizeof(char));

	return buffer;
}

FILE * open_file_read(char *file_name){

    FILE * file_in;
    file_in = fopen(file_name, "r");
    if (file_in == NULL){

    	fprintf(stderr, "Arquivo %s nao encontrado!\n", file_name);
    }
    return file_in;
}

void start_chat(int sock_fd, struct sockaddr_in *peer)
{

	struct sockaddr_in cli_addr;
  socklen_t slen=sizeof(cli_addr);
  int ret;
  ssize_t bytes;
  char input_buffer[BUFFERSIZE];
  char output_buffer[BUFFERSIZE];
  struct pollfd fds[2];

  /* Descriptor zero is stdin */
  fds[0].fd = 0;
  fds[1].fd = sock_fd;
  fds[0].events = POLLIN | POLLPRI;
  fds[1].events = POLLIN | POLLPRI;

  /* Normally we'd check an exit condition, but for this example
   * we loop endlessly.
   */
  while (1) {
    /* Call poll() */
    ret = poll(fds, 2, -1);

    if (ret < 0) {
      printf("Error - poll returned error: %s\n", strerror(errno));
      break;
    }
    if (ret > 0) {

      /* Regardless of requested events, poll() can always return these */
      if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        printf("Error - poll indicated stdin error\n");
        break;
      }
      if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        printf("Error - poll indicated socket error\n");
        break;
      }

      /* Check if data to read from stdin */
      if (fds[0].revents & (POLLIN | POLLPRI)) {
        bytes = read(0, output_buffer, sizeof(output_buffer));
        if (bytes < 0) {
          printf("Error - stdin error: %s\n", strerror(errno));
          break;
        }
				char *buf = (char *) malloc((int)bytes);
   			sprintf(buf, "%.*s",(int)(bytes-1), output_buffer);
				if(strcmp(buf, "TCHAU") != 0){
	        printf("Sending: %.*s\n", (int)bytes, output_buffer);

  		    bytes = sendto(sock_fd, output_buffer, bytes, 0,
  	                     (struct sockaddr *)peer, sizeof(struct sockaddr_in));
				}else{
						printf("Saindo...\n");
						break;
				}
  	    if (bytes < 0) {
  	      printf("Error - sendto error: %s\n", strerror(errno));
  	      break;
	      }
				free(buf);
      }

      /* Check if data to read from socket */
      if (fds[1].revents & (POLLIN | POLLPRI)) {
        bytes = recvfrom(sock_fd, input_buffer, sizeof(input_buffer),
                         0, (struct sockaddr*)&cli_addr, &slen);
        if (bytes < 0) {
          printf("Error - recvfrom error: %s\n", strerror(errno));
          break;
        }
        if (bytes > 0) {
					*peer = cli_addr;
          printf("Received from %s:%d: %.*s\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), (int)bytes, input_buffer);
				}
      }
    }
  }

}

void err(char *s)
{
    perror(s);
    exit(1);
}

void clienteUDP(char *ip, int PORT){ //Cliente que está conversando status = 1
    struct sockaddr_in serv_addr;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_aton(ip, &serv_addr.sin_addr)==0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        return;
    }

	  start_chat(sockfd, &serv_addr);

}

void servidorUDP(int PORT){ //Cliente que está disponível status = 2 
    struct sockaddr_in my_addr, cli_addr;
    int sockfd, bytes;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFFERSIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      err("socket");

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
      err("bind");
    else
      printf("Server port: %d : successful\n", PORT);
		if ((bytes = recvfrom(sockfd, buf, BUFFERSIZE, 0, (struct sockaddr*)&cli_addr, &slen))!=-1){
			printf("Received from %s:%d: %.*s\n",inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), (int)bytes, buf);
			start_chat(sockfd, &cli_addr);
		}

}
