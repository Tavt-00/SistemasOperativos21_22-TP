
#include "util.h"

int main(int argc, char *argv[])
{
	setbuf(stdout,NULL);

	int fd_cli_fifo, fd_balc_fifo, fd_medic_fifo;
	char nome_fifo_cli[30], nome_fifo_med[30];
	int response_select, res_size, pid;
	char comando[30], nome[30];
	int recebeuDadosCli = 0, primeiraVez = 0;

	fd_set fds;
	struct timeval tempo;
	
	medico med_dados;
	user cli_dados;


	if (argc < 2)
	{
		printf("[ERRO] Parametros em falta\n");
		exit(1);
	}

	//guardar dados do medico
	pid = getpid();
	med_dados.med_id = pid;
	med_dados.flagRegistado = 0;
	strcpy(nome, argv[1]);
	strcpy(med_dados.especialidade, argv[2]);
	sprintf(nome_fifo_med, MEDICO_FIFO, med_dados.med_id);
	
	strcpy(med_dados.nome, nome);

	//abrir fifo do balcao para escrita
	fd_balc_fifo = open(BALCAO_FIFO2, O_WRONLY);
	if(fd_balc_fifo == -1){
		printf("[ERRO] Abrir o fifo do balcao\n");
		exit(2);
	}


	//ver se ja existe fifo, senao crio
	if(access(nome_fifo_med,F_OK) != 0){
		if(mkfifo(nome_fifo_med, 0600) == -1){
			exit(2);
		}
	}

	fd_medic_fifo = open(nome_fifo_med, O_RDWR);
	if(fd_medic_fifo == -1){
		printf("[ERRO] Abrir o FIFO do Medico em questao\n");
		close(fd_medic_fifo);
		exit(2);
	}

	printf("Ola Sr. %s, aguarde...\n", med_dados.nome);

	//enviar info do medico ao balcao
	write(fd_balc_fifo, &med_dados, sizeof(medico));


	while (1)
	{
		FD_ZERO(&fds);	//vindo do fifo	
		FD_SET(0, &fds);	//teclado
		FD_SET(fd_medic_fifo, &fds);	
		tempo.tv_sec = 10;		  
		tempo.tv_usec = 0;	

		response_select = select(fd_medic_fifo + 1, &fds, NULL, NULL, &tempo);
		
		if(response_select > 0 && FD_ISSET(fd_medic_fifo, &fds))	//ler o que vem no seu fifo
		{
			res_size = read(fd_medic_fifo, &med_dados, sizeof(medico));
			if (res_size != sizeof(medico))
			{
				printf("[ERRO] Ler do fifo medico\n");
				exit(3);
			}

			if (res_size != sizeof(medico))
			{
				printf("[ERRO] Ler no fifo do cliente\n");
				exit(3);
			}
			if (strcmp(med_dados.mensagem, "ENCERRA") == 0)
			{ 
				printf("[AVISO] O balcao encerrou!\n");
				break;
			}
			else if(strcmp(med_dados.mensagem, "MAXMED") == 0){
				printf("Maximo de medicos foi atingido, tente mais tarde\n");
				break;
			}
			else if (strcmp(med_dados.mensagem, "BALC_OK") == 0)	//esta a falar com o balcao
			{ 
				printf("Registado pelo balcao, aguarde...\n");
			}
			else if (strcmp(med_dados.mensagem, "RECEBE_DADOS_CONS") == 0)	//
			{
				recebeuDadosCli = 1;

				printf("\n__Atencao, o utente %s ira ser-lhe atribuido__\n", med_dados.cliente_atual.nome);
				printf("Pode comecar a falar com o utente\n");

				sprintf(nome_fifo_cli, UTENTE_FIFO, med_dados.cliente_atual.pidUt);
				fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
				if (fd_cli_fifo == -1)
				{
					printf("[ERRO] Abrir o fifo do utente para a consulta");
					close(fd_cli_fifo);
					exit(2);
				}
								
			}
			else if (strcmp(med_dados.mensagem, "CONSULTA") == 0)	//
			{
				printf("\n-Utente: %s\n", med_dados.chat);
			}
			else if (strcmp(med_dados.mensagem, "ADEUS") == 0){
				printf("O seu utente decidiu encerrar a consulta\n");
				
			}
		}
		else if (response_select > 0 && FD_ISSET(0, &fds))	//ler do teclado
		{
			fgets(comando, sizeof(comando), stdin);

			if(strcmp(comando, "adeus\n") == 0){
				strcpy(cli_dados.mensagem, "ADEUS");
				write(fd_cli_fifo, &cli_dados, sizeof(user));
				primeiraVez = 0;
				recebeuDadosCli = 0;
				
				//avisar balcao
				strcpy(med_dados.mensagem, "ADEUS");
				med_dados.med_id = pid;
				write(fd_balc_fifo, &med_dados, sizeof(medico));
			}	
			else if(strcmp(comando, "sair\n") == 0){
				strcpy(med_dados.mensagem, "SAIR");
				med_dados.med_id = pid;
				write(fd_balc_fifo, &med_dados, sizeof(medico));
				break;
			}
			else if(recebeuDadosCli == 1){
				if (!primeiraVez)
				{
					cli_dados.pid_medAtr = pid;
					strcpy(cli_dados.medico_atr, nome);
					strcpy(cli_dados.chat, comando);
					strcpy(cli_dados.mensagem, "INICIO_CONS");

					write(fd_cli_fifo, &cli_dados, sizeof(user));

					primeiraVez = 1;
				}
				else if(primeiraVez){
					strcpy(cli_dados.chat, comando);
					strcpy(cli_dados.mensagem, "CONSULTA");
					write(fd_cli_fifo, &cli_dados, sizeof(user));

				}	
			}	
			
		}
	}
	

	printf("[AVISO] Medico a encerrar...\n");
	close(fd_medic_fifo);
	close(fd_balc_fifo);
	close(fd_cli_fifo);
	unlink(nome_fifo_med);

	exit (0);
}