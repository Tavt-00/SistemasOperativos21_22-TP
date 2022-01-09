
#include "util.h"

int main(int argc, char *argv[])
{
	setbuf(stdout,NUll);

	medido med_dados;
	cliente paci_dados;
	int flagTerm = 0;

	int fd_cli_fifo, fd_balc_fifo, fd_medic_fifo;
	char nome_fifo_cli[30], nome_fifo_med[30];
	int res_read, res_wr;

	if (argc < 2)
	{
		printf("[ERRO] Parametros em falta\n");
		exit(1);
	}

	//guardar dados do medico
	med_dados.med_id = getpid();
	strcpy(med_dados.nome, argv[1]);
	strcpy(med_dados.especialidade, argv[2]);
	sprintf(nome_fifo_med, MED_FIFO, med_dados.med_id);
	

	//abrir fifo do balcao para escrita
	fd_balc_fifo = open(BALCAO_FIFO, O_WRONLY);

	if(fd_balc_fifo == -1){
		printf("[ERRO] Abrir o FIFO do Árbitro\n");
		exit(2);
	}


	//ver se ja existe fifo, senao crio
	if(access(nome_fifo_med,F_OK) != 0)
		mkfifo(nome_fifo_med, 0600);


	// abro o fifo do cliente como RDWR, bloqueante
	fd_medic_fifo = open(nome_fifo_med, O_RDWR);
	if(fd_medic_fifo == -1){
		printf("[ERRO] Abrir o FIFO do Medico em questao\n");
		close(fd_medic_fifo);
		exit(2);
	}

	prinft(" %s (%s) resgistado(a) com sucesso! \n", med_dados.nome, med_dados.especialidade);


	//enviar info do medico ao balcao para por na base de dados dos medicos disponiveis
	write(fd_balc_fifo, &med_dados, sizeod(medido));


	do{

	//fazer um read de modo a ficar à espera da atribuição do prox cliente pelo balcao
	res = read(nome_fifo_med, &paci_dados, sizeof(cliente));

		//verificar o conteudo da resposta
	//if(res == sizeof(cliente))
		printf("\n Nome paciente: %s, Sintomas: %s, Prioridade: %d", paci_dados.nome, paci_dados.sintoma, paci_dados.prioridade_atr);
	

		while(1){

			//esperar por user input e por pipe




			if(flagTerm == 1){
				break;
			}
		}

	}while(flagTerm == 0);


	return 0;
}