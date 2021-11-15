#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "cliente.h"

int main(int argc, char *argv[])
{
	int fd_cli_fifo, fd_balc_fifo, fd_medic_fifo;
	int res_read, res_wr;
	char nome_fifo_cli[30], nome_fifo_medic[30];
	user cli_dados, cli_resposta;	


	//vê argumentos
	if (argc < 2)
	{
		printf("[ERRO] Parametros em falta! \n");
		exit(1);
	}

	cli_dados.user_id = getpid();

	//guardar nome e criar fifo fo utente
	strcpy(cli_dados.nome, argv[1]);
	sprintf(nome_fifo_cli, UTENTE_FIFO, cli_dados.user_id);


	//ver se já existe fifo, senao cria
	if (access(nome_fifo_cli,F_OK) != 0)
	{
		if(mkfifo(nome_fifo_cli, 0600) == -1)
			exit(2);
	}

	//abrir fifo para escrever no balcao
	fd_balc_fifo = open(BALCAO_FIFO, O_WRONLY);
	if(fd_balc_fifo == -1){
 		printf("[ERRO] Abrir o fifo do balcao\n");
 		exit(2);
	}
	
			printf("FIFO do Balcao aberto WRITE / BLOCKING");		//APAGAR DEPOIS

	//abrir fifo do cliente em questão
	fd_cli_fifo = open(nome_fifo_cli, O_RDWR);
 	if(fd_cli_fifo == -1){
 		perror("[ERRO] Abrir o fifo do cliente em questao");
 		close(fd_cli_fifo);
 		exit(2);
 	}

 			printf("FIFO do utente aberto para READ(+Write) Block"); //APAGAR DEPOIS

 	//comunicação BALCAO - UTENTE

 	//pedir sintomas 
 	printf("Utente %s, coloque aqui os seus sintomas: ", cli_dados.nome);
 	scanf("%s", cli_dados.sintoma);

 	//enviar ao balcao
 	res_wr = write(fd_balc_fifo, &cli_dados, sizeof(cli_dados));
 	if(res_wr != sizeof(cli_dados)){
 		printf("[ERRO] Enviar do utente para balcao\n");
 		exit(3);
 	}

 	//ler do balcao
 	res_read = read(fd_balc_fifo, &cli_resposta, sizeof(cli_resposta));
 	if(res_read != sizeof(cli_resposta)){
 		printf("[ERRO] Ler do balcao para o utente\n");
 		exit(3);
 	}

 	close(fd_cli_fifo);
 	close(fd_balc_fifo);
 	unlink(nome_fifo_cli);
 	exit(0);

}





