
#include "util.h"

int main(int argc, char *argv[])
{

	setbuf(stdout, NULL);
	int fd_cli_fifo, fd_balc_fifo, fd_medic_fifo;
	int res_read, res_wr;
	char nome_fifo_cli[30], nome_fifo_medic[30];
	int flagConsulta = 0, flagFase = 0, recebeuMedico = 0; //para saber quem está a mandar mensagem
	int res_size, response_select, pid;
	char comando[30], especialidade[30];

	fd_set fds;
	struct timeval tempo;

	user cli_dados;
	medico med_dados;


	//vê argumentos
	if (argc < 2)
	{
		printf("[ERRO] Parametros em falta! \n");
		exit(1);
	}

	if (access(BALCAO_FIFO, F_OK))
	{
		printf("[AVISO] Balcao nao se encontra a correr\n");
		exit(1);
	}

	cli_dados.pidUt = getpid();
	pid = cli_dados.pidUt;
	cli_dados.flagRegistado = 0;

	//guardar nome e criar fifo fo utente
	strcpy(cli_dados.nome, argv[1]);
	sprintf(nome_fifo_cli, UTENTE_FIFO, cli_dados.pidUt);

	//ver se já existe fifo do cliente, senao cria
	if (access(nome_fifo_cli, F_OK) != 0)
	{
		if (mkfifo(nome_fifo_cli, 0600) == -1)
			exit(2);
	}

	//abrir fifo cliente para ler
	fd_cli_fifo = open(nome_fifo_cli, O_RDWR);
	if (fd_cli_fifo == -1)
	{
		printf("[ERRO] Abrir o fifo do cliente em questao");
		close(fd_cli_fifo);
		exit(2);
	}

	//printf("FIFO do utente aberto para READ(+Write) Block"); //APAGAR DEPOIS

	//abrir fifo para escrever no balcao
	fd_balc_fifo = open(BALCAO_FIFO, O_RDWR);
	if (fd_balc_fifo == -1)
	{
		printf("[ERRO] Abrir o fifo balcao");
		close(fd_balc_fifo);
		exit(2);
	}


	printf("Utente %s, diga o seu sintoma: ", cli_dados.nome);

	fgets(cli_dados.mensagem, sizeof(cli_dados.mensagem), stdin);
	//enviar ao balcao
	write(fd_balc_fifo, &cli_dados, sizeof(user));

	while (1)
	{ 

		FD_ZERO(&fds);			   
		FD_SET(0, &fds);		   
		FD_SET(fd_cli_fifo, &fds); 
		tempo.tv_sec = 10;		   
		tempo.tv_usec = 0;

		response_select = select(fd_cli_fifo + 1, &fds, NULL, NULL, &tempo);

		if (response_select > 0 && FD_ISSET(fd_cli_fifo, &fds)) 
		{ 		
			res_size = read(fd_cli_fifo, &cli_dados, sizeof(user));

			if (res_size != sizeof(user))
			{
				printf("[ERRO] Ler no fifo do cliente\n");
				exit(3);
			}
			if (strcmp(cli_dados.mensagem, "ENCERRA") == 0)
			{ 
				printf("[AVISO] O balcao encerrou!\n");
				break;
			}
			else if(strcmp(med_dados.mensagem, "MAXCLI") == 0){
				printf("Maximo de utentes totais foi atingido, tente mais tarde\n");
				break;
			}
			else if (strcmp(cli_dados.mensagem, "BALC_FILA_OK") == 0)	
			{ 
				printf("\nFoi registado e adicionado na fila com sucesso! Tem %d utentes na sua frente. Aguarde pelo medico...\n", cli_dados.numUtFrente);
				printf("Especialidade atribuida: %s. Prioridade igual a: %d\n", cli_dados.espec_atr, cli_dados.prioridade_atr);
				strcpy(especialidade , cli_dados.espec_atr);
			}
			else if (strcmp(cli_dados.mensagem, "BALC_FILA_NO_OK") == 0){
				printf("A fila da sua especialidade [%s] esta cheia, dado a sua prioridade [%d] nao ha mais espaco.\n",cli_dados.espec_atr, cli_dados.prioridade_atr);
				printf("Lamentamos, nao foi possivel fazer o seu registado. Volte mais tarde.\n");
				break;
			}
			else if (strcmp(cli_dados.mensagem, "BALC_FILA_NOMORE_OK") == 0){
				printf("\nLamentamos, chegaram novos utentes para a sua fila com prioridade superior a sua!\n");
				printf("Vai ser retirado da fila. Volte mais tarde.\n");
				break;
			}
			else if (strcmp(cli_dados.mensagem, "DELEUT") == 0)	//esta a falar com o balcao
			{ 
				printf("\n\n[AVISO] Foi removido pelo balcao\n");
				break;
			}
			else if (strcmp(cli_dados.mensagem, "INICIO_CONS") == 0)	//esta a falar com o medico
			{
				recebeuMedico = 1;

				sprintf(nome_fifo_medic, MEDICO_FIFO, cli_dados.pid_medAtr);
				fd_medic_fifo = open(nome_fifo_medic, O_WRONLY);
				if (fd_medic_fifo == -1)
				{
					printf("[ERRO] Abrir o fifo do medico para a consulta\n");
					close(fd_medic_fifo);
					exit(2);
				}

				printf("\n[AVISO] Atencao, o medico %s vai falar consigo\n", cli_dados.medico_atr);
				printf("Pode comecar a falar com o medico\n");
				printf("\n-Medico: %s\n", cli_dados.chat);

			}
			else if (strcmp(cli_dados.mensagem, "CONSULTA") == 0){
				printf("\n-Medico: %s\n", cli_dados.chat);
			}
			else if (strcmp(cli_dados.mensagem, "ADEUS") == 0){
				
				printf("\n[AVISO] O seu medico decidiu encerrar a consulta. O programa vai encerrar.\n");
				
				strcpy(cli_dados.mensagem, "ADEUS");
				cli_dados.flagRegistado = -1;
				cli_dados.pidUt = pid;
				
				write(fd_balc_fifo, &cli_dados, sizeof(user));
				break;
			}
		}
		else if (response_select > 0 && FD_ISSET(0, &fds)) //ler do teclado
		{ //se recebeu algo do teclado
			
			fgets(comando, sizeof(comando), stdin);

			if(strcmp(comando, "adeus\n") == 0){
				strcpy(med_dados.mensagem, "ADEUS");
				write(fd_medic_fifo, &med_dados, sizeof(medico));

				strcpy(cli_dados.mensagem, "ADEUS");
				cli_dados.pidUt = pid;
				cli_dados.flagRegistado = -1;
				strcpy(cli_dados.espec_atr, especialidade); 
				write(fd_balc_fifo, &cli_dados, sizeof(user));
				break;
			}	
			else if(strcmp(comando, "sair\n") == 0 && recebeuMedico == 0){
				strcpy(cli_dados.mensagem, "SAIR");
				cli_dados.flagRegistado = -1;
				cli_dados.pidUt = pid;
				strcpy(cli_dados.espec_atr, especialidade); 
				write(fd_balc_fifo, &cli_dados, sizeof(user));
				break;
			}
			else if(strcmp(comando, "sair\n") == 0 && recebeuMedico == 1){
				printf("\n[AVISO] Atencao, encontra-se numa consulta! Para sair escreva o comando 'adeus'.\n\n");
			}
			else if(recebeuMedico == 1){
				
				strcpy(med_dados.mensagem, "CONSULTA");
				strcpy(med_dados.chat, comando);

				write(fd_medic_fifo, &med_dados, sizeof(medico));
			}
		}
	}

	//avisar balcao que vai sair

	printf("\n[AVISO] Cliente a encerrar...\n");

	close(fd_cli_fifo);
	close(fd_balc_fifo);
	close(fd_medic_fifo);
	unlink(nome_fifo_cli);

	exit(0);
}
