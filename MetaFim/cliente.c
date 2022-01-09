
#include "util.h"

int main(int argc, char *argv[])
{

	setbuf(stdout,NULL);
	int fd_cli_fifo, fd_balc_fifo, fd_medic_fifo;
	int res_read, res_wr;
	char nome_fifo_cli[30], nome_fifo_medic[30];
	int flagConsulta = 0, flagFase = 0; //para saber quem está a mandar mensagem
	int res_size, response_select;
	char comando[30];

	fd_set fds;
	struct timeval tempo;

	balcUt balcUtData;
	utMed UtMedData;
	user cli_dados;

	//vê argumentos
	if (argc < 2)
	{
		printf("[ERRO] Parametros em falta! \n");
		exit(1);
	}


	if(access(BALCAO_FIFO, F_OK)){
		printf("[AVISO] Balcao nao se encontra a correr\n");
		exit(1);
	}

	cli_dados.pidUt = getpid();
	//balcUtData.pidUt = getpid();

	//guardar nome e criar fifo fo utente
	strcpy(cli_dados.nome, argv[1]);
	sprintf(nome_fifo_cli, UTENTE_FIFO, cli_dados.pidUt);

	//ver se já existe fifo do cliente, senao cria
	if (access(nome_fifo_cli, F_OK) != 0){
		if(mkfifo(nome_fifo_cli, 0600) == -1)
			exit(2);
	}
	
	printf("1\n");
	//abrir fifo cliente para ler 
	fd_cli_fifo = open(nome_fifo_cli, O_RDWR);
 	if(fd_cli_fifo == -1){
 		printf("[ERRO] Abrir o fifo do cliente em questao");
 		close(fd_cli_fifo);
 		exit(2);
 	}
 		printf("nome fifo: %s \n", nome_fifo_cli);

 	//printf("FIFO do utente aberto para READ(+Write) Block"); //APAGAR DEPOIS

 	//abrir fifo para escrever no balcao
	fd_balc_fifo = open(BALCAO_FIFO, O_RDWR);
	if(fd_balc_fifo == -1){
 		printf("[ERRO] Abrir o fifo balcao");
 		close(fd_balc_fifo);
 		exit(2);
 	}
 	else{
 		printf("13333\n");
 	}

 	printf("Utente %s, diga o seu sintoma: ", cli_dados.nome);

 	fgets(cli_dados.mensagem, sizeof(cli_dados.mensagem), stdin); 
 	//enviar ao balcao
	write(fd_balc_fifo, &cli_dados, sizeof(user));

 	while(1){  //select


		FD_ZERO(&fds); // limpar os file descriptors do select
		FD_SET(0,&fds); // atribui ao STDIN
		FD_SET(fd_cli_fifo, &fds); // olhar para o FIFO
		tempo.tv_sec = 10;	//qt tempo queremos ficar parados à espera, 10s neste caso
		tempo.tv_usec = 0;

		response_select = select(fd_cli_fifo+1,&fds,NULL, NULL, &tempo);

		//if(flagConsulta == 0){ //se não estiver em consulta, lê e escreve uma struct balcUt //usar estrutura do cliente

				printf("entrei1\n");

			if(response_select > 0 && FD_ISSET(fd_cli_fifo,&fds) ){ //se recebeu algo do balcao


				//cliente lê resposta vinda do balcao
				res_size = read(fd_cli_fifo, &cli_dados, sizeof(user));
				if(res_size != sizeof(user)){
 					printf("[ERRO] Ler do balcao para o utente\n");
 					exit(3);
 				}
 				printf("Li a resposta: %s e prio %d\n",cli_dados.espec_atr, cli_dados.prioridade_atr );
 				printf("Cli mensage: %s\n", cli_dados.mensagem);

				if(strcmp(cli_dados.mensagem, "FECHAR") == 0){ //quando o balcao fecha manda mensagem para o cliente
					
					printf("asd\n");
				}
				else if(strcmp(cli_dados.mensagem, "CLASS") == 0){ //classificação mandada pelo balcao

					printf("Li a resposta: %s e prio %d\n",cli_dados.espec_atr, cli_dados.prioridade_atr );
				}

			}else if(response_select > 0 && FD_ISSET(0,&fds)){ //se recebeu algo do teclado
				printf("Entrei no teclado\n");
				fgets(comando, strlen(comando), stdin); 

		}
	}



 	close(fd_cli_fifo);
 	close(fd_balc_fifo);
 	unlink(nome_fifo_cli);
 	exit(0);

}





