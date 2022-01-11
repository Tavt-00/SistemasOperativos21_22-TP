
#include "util.h"

int main(int argc, char *argv[])
{
	setbuf(stdout,NULL);

	int fd_cli_fifo, fd_balc_fifo, fd_medic_fifo;
	char nome_fifo_cli[30], nome_fifo_med[30];
	int response_select, res_size;
	char comando[30];



	fd_set fds;
	struct timeval tempo;
	
	medico med_dados;
	user paci_dados;
	balcMed balcMedData;

	if (argc < 2)
	{
		printf("[ERRO] Parametros em falta\n");
		exit(1);
	}

	//guardar dados do medico
	med_dados.med_id = getpid();
	med_dados.flagRegistado = 0;
	strcpy(med_dados.nome, argv[1]);
	strcpy(med_dados.especialidade, argv[2]);
	sprintf(nome_fifo_med, MEDICO_FIFO, med_dados.med_id);
	

	//abrir fifo do balcao para escrita
	fd_balc_fifo = open(BALCAO_FIFO2, O_WRONLY);
	if(fd_balc_fifo == -1){
		printf("[ERRO] Abrir o FIFO do Árbitro\n");
		exit(2);
	}


	//ver se ja existe fifo, senao crio
	if(access(nome_fifo_med,F_OK) != 0){
		if(mkfifo(nome_fifo_med, 0600) == -1){
			exit(2);
		}
	}

	// abro o fifo do cliente como RDWR, bloqueante
	fd_medic_fifo = open(nome_fifo_med, O_RDWR);
	if(fd_medic_fifo == -1){
		printf("[ERRO] Abrir o FIFO do Medico em questao\n");
		close(fd_medic_fifo);
		exit(2);
	}

	printf(" Ola Sr. %s, aguarde...\n", med_dados.nome);

	printf("Fifo: %s", nome_fifo_med);

	//enviar info do medico ao balcao
	write(fd_balc_fifo, &med_dados, sizeof(medico));


	while (1)
	{
		FD_ZERO(&fds);	//vindo do fifo	
		FD_SET(0, &fds);	//teclado
		FD_SET(fd_medic_fifo, &fds);	
		tempo.tv_sec = 10;		  
		tempo.tv_usec = 0;	

	printf("ola\n");
		response_select = select(fd_medic_fifo + 1, &fds, NULL, NULL, &tempo);
		
		if(response_select > 0 && FD_ISSET(fd_medic_fifo, &fds))	//ler o que vem no seu fifo
		{
			printf("olaaaa\n");

			res_size = read(fd_medic_fifo, &med_dados, sizeof(medico));
			if (res_size != sizeof(medico))
			{
				printf("[ERRO] Ler do fifo medico\n");
				exit(3);
			}
			printf("MEDDADOS : \n%s",med_dados.mensagem );

			if (res_size != sizeof(medico))
			{
				printf("[ERRO] Ler no fifo do cliente\n");
				exit(3);
			}
			if (strcmp(med_dados.mensagem, "FECHAR") == 0)
			{ //quando o balcao fecha manda mensagem para o cliente

				printf("asd\n");
			}
			else if (strcmp(med_dados.mensagem, "BALC") == 0)	//esta a falar com o balcao
			{ 
				printf("Registado pelo balcao, aguarde...\n");
			}
			else if (strcmp(med_dados.mensagem, "CONSULTA") == 0)	//na consulta
			{
				printf("asd22\n");
			}

		}
		else if (response_select > 0 && FD_ISSET(0, &fds))	//ler do teclado
		{
			printf("Entrei no teclado\n");
			fgets(comando, strlen(comando), stdin);
		}
		
	}
	

	close(fd_medic_fifo);
	close(fd_balc_fifo);
	unlink(nome_fifo_med);

	exit (0);
}