#include "util.h"

/*
//alocar memória para novos utentes
utente_ptr add_ut(int *numUt, utente_ptr array_ut, user_ptr new_user){

	if(*numUt == 0){
		array_ut = malloc(sizeof(utente));
		if(array_ut == NULL){
    		printf("[ERRO1] Alocacao de memoria\n");
	    	free(array_ut);
	    	return(NULL);
		}
	}else{

		utente_ptr aux;
		aux = realloc(array_ut, sizeof(utente)*(*numUt + 1));
        if(aux == NULL){
            printf("[ERRO2] Realocacao de memoria\n");
            free(aux);
            return(NULL);
        }

        array_ut = aux;
    }


    //Copiar a informacao
	strcpy(array_ut[*numUt].cliente.nome, new_user->nome);
	array_ut[(*numUt)].cliente.user_id = new_user->user_id; 
				
	array_ut[(*numUt)].flag_emConsulta = 0;	//ainda não está em consulta

	(*numUt)++;
	return array_ut;
}


//remover cliente do array dinâmico
utente_ptr remover_ut(int *numUt, utente_ptr array_ut, user_ptr old_user, int pos){

	if(*numUt > 1){

		if(pos != *numUt-1)
			array_ut[pos] = array_ut[*numUt-1];

		array_ut aux;
		aux = realloc(array_ut, sizeof(utente)*(numUt-1));
		if(aux == NULL){
			printf("[ERRO] na realocacao de memoria");
			free(aux);
			return NULL;
		}

		array_ut = aux;
		(*numUt)--;

		return array_ut;
	}	
	else if (*numUt == 1){
		free (array_ut);
		*numUt = 0;
		return NULL;
	}
}


//listar clientes
void lista_cli(utente_ptr array_ut, int numUt){

	printf("Existem %d utentes\n", numUt);

	for(int i = 0; i< numUt; i++)
		printf("Nome Utente: [%s] - ID Utente: [%d] - Espec atribuida: [%s] - Prioridade: [%d] \n", array_ut[i].cliente.nome, array_ut[i].cliente.user_id, array_ut[i].cliente.espec_atr, array_ut[i].cliente.prioridade_atr);
}


//listar medicos
void lista_cli(med_ptr array_med, int numMed){

	printf("Existem %d medicos\n", numUt);

	for(int i = 0; i< numUt; i++)
		printf("Nome Medico: [%s] - ID Medico: [%d] - Especialidade: [%s]\n", array_med[i].medico_esp.nome, array_med[i].medico_esp.med_id, array_med[i].medico_esp.especialidade);
}


// adicionar medicos

med_ptr add_med(int *numMed, med_ptr array_med, medico_ptr new_med){

	if(*numUt == 0){
		array_med = malloc(sizeof(med));
		if(array_ut == NULL){
    		printf("[ERRO1] Alocacao de memoria\n");
	    	free(array_med);
	    	return(NULL);
		}
	}else{

		med_ptr aux;
		aux = realloc(array_med, sizeof(med)*(*numMed + 1));
        if(aux == NULL){
            printf("[ERRO2] Realocacao de memoria\n");
            free(aux);
            return(NULL);
        }

        array_med = aux;
    }


    //Copiar a informacao
	strcpy(array_med[*numUt].medico_esp.nome, new_med->nome);
	array_med[(*numMed)].medico_esp.med_id = new_med->med_id; 
				
	array_med[(*numMed)].flag_emConsulta = 0;	//ainda não está em consulta

	(*numMed)++;
	return array_Med;
}

//remover medicos 
med_ptr remover_med(int *numMed, med_ptr array_med, int pos){

	if(*numMed > 1){

		if(pos != *numMed-1)
			array_med[pos] = array_med[*numMed-1];

		array_med aux;
		aux = realloc(array_med, sizeof(medico)*(numMed-1));
		if(aux == NULL){
			printf("[ERRO] na realocacao de memoria");
			free(aux);
			return NULL;
		}

		array_med = aux;
		(*numMed)--;

		return array_ut;
	}	
	else if (*numMed == 1){
		free (array_med);
		*numMed = 0;
		return NULL;
	}
}


//avanca fila 
void avancaFila(utente_ptr array_ut, med_ptr array_med, int numUt, int numMed, char *espec ){

	balcMed balcMedAux;
	balcUt balcUtAux;


	for(int i = 0; i < numUt; i++){

		if(array_ut[i].flag_emConsulta == 0 && strcmp(array_ut[i].cliente.espec_atr, espec)){
			if(array_ut[i].cliente.numUtente_frente > 0) {
				array_ut[i].cliente.numUtente_frente--;
			}
			else if(array_ut[i].cliente.numUtente_frente==0){ //se for o proximo a ser atendido

				for (int j = 0; j < numMed ; ++i){
					
					//procurar medico livre 
					//se nao tiver em consulta e for da especialidade pretendida
					if(array_med[i].flag_emConsulta == 0 && strcmp(array_med[i].medico_esp.especialidade, espec)){

						//associar o medico atribuido ao cliente e vice-versa (se interessar saber isso no balcao)
						//array_med[i].medico_esp.cliente_atual = array_ut[i].cliente;

						//mandar informações do medico para o utente
						int cli_pid = array_ut[i].client.user_id;
						char cli_fifo[40];
						sprintf(cli_fifo,UTENTE_FIFO,cli_pid);
						int fd_cli_fifo = open(cli_fifo,O_WRONLY);

						//preencher a estrutura balcUt com informações do medico
						balcUtAux.pid_med_atr = array_med[i].med_id;	//falta adicionar na struct
						write(fd_cli_fifo, &balcUtAux, sizeof(balcUt));

						//mandar informações do utente para o medico
						int med_pid = array_med[i].med_esp.user_id;
						char med_fifo[40];
						sprintf(med_fifo,MED_FIFO,med_pid);
						int fd_med_fifo = open(med_fifo,O_WRONLY);

						//preencher a estrutura balcMed com informações do cliente
						balcMedAux.info_ut_atr = array_ut[i].cliente;	//falta adicionar na struct
						write(fd_med_fifo, &balcMedAux, sizeof(balcMed));


						close(fd_client_fifo);
						close(fd_med_fifo);
					}
				
				}

			}

		}
		
	}

}

*/

void *threadClassCli(void *dados)
{

	int res, prioLida, estadoFilho;
	int fd_cli_fifo, fd_balc_fifo, res_size;
	int balClass[2], classBalc[2];
	char class_info[30], especLida[30], fimClass[5] = "#fim\n";
	char nome_fifo_cli[30];

	balcUt *balcUtData = (balcUt *)dados;
	user cli_dados;

	//tratar classificador

	pipe(balClass);
	pipe(classBalc);

	res = fork();
	if (res == 0)
	{ //filho

		//stout do classificador
		close(1);
		dup(classBalc[1]);
		close(classBalc[1]);
		close(classBalc[0]);

		//stdin para o classificador
		close(0);
		dup(balClass[0]);
		close(balClass[1]);
		close(balClass[0]);

		execl("classificador", "classificador", NULL);
		//se vier para aqui o exec correu mal
		printf("Exec correu mal!");
		exit(123);
	}

	close(balClass[0]);
	close(classBalc[1]);

	//tratar fifo balcao

	fd_balc_fifo = open(BALCAO_FIFO, O_RDWR);
	if (fd_balc_fifo == -1)
	{
		printf("[ERRO] Abrir o fifo do balcao\n");
		exit(2);
	}

	//le sintomas do cliente
	res_size = read(fd_balc_fifo, &cli_dados, sizeof(user));
	if (res_size != sizeof(user))
	{
		printf("[ERRO] Ler do fifo balcao\n");
		exit(3);
	}

	sprintf(nome_fifo_cli, UTENTE_FIFO, cli_dados.pidUt);

	fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
	if (fd_cli_fifo == -1)
	{
		printf("[ERRO] Abrir o fifo do balc para o cliente\n");
		exit(2);
	}

	//manda sintomas para o classificador
	write(balClass[1], &(cli_dados.mensagem), strlen(cli_dados.mensagem));

	//le resposta do classificador
	read(classBalc[0], class_info, sizeof(class_info));

	//guardar resposta do class no user
	sscanf(class_info, "%s %d", cli_dados.espec_atr, &cli_dados.prioridade_atr);
	strcpy(cli_dados.mensagem, "CLASS");

	printf("Especialidade atribuida: %s\nPrioridade atribuida: %d\n", cli_dados.espec_atr, cli_dados.prioridade_atr);

	write(fd_cli_fifo, &cli_dados, sizeof(user));

	//criar funcao para guardar a estrutura clidados para a balcUTdata
	//usar mutex
	//newUser.prioridade_atr = prioLida;
	//strcpy(newUser.espec_atr, especLida);

	write(balClass[1], fimClass, strlen(fimClass)); //avisar classificador para terminar

	wait(&estadoFilho);

	pthread_exit(NULL);
}

int main(int argc, char *argv[], char *envp[])
{
	setbuf(stdout, NULL);

	int res_size, flag_fimClass = 0;
	int fd_balc_fifo, fd_cli_fifo;
	char comando[30], sendExit = 'E';

	char *maxClientes, *maxMedicos;
	int max_clientes, max_medicos;

	user newUser;
	utente_ptr array_ut = NULL; //ponteiro para array de clientes
	balcUt balcUtData;

	pthread_t thread_classCli;
	pthread_mutex_t trinco;

	//ver fifo balcao
	if (access(BALCAO_FIFO, F_OK) != 0)
	{
		mkfifo(BALCAO_FIFO, 0600);
	}

	//----Tratar Variáveis Ambiente-------

	maxClientes = getenv("MAXCLIENTES");
	if (maxClientes == NULL)
	{
		printf("[Erro] MAXCLIENTES nao existe!\n");
		exit(1);
	}
	else
	{
		max_clientes = atoi(maxClientes);
	}

	maxMedicos = getenv("MAXMEDICOS");
	if (maxMedicos == NULL)
	{
		printf("[Erro] MAXMEDICOS nao existe!\n");
		exit(1);
	}
	else
	{
		max_medicos = atoi(maxMedicos);
	}

	fd_balc_fifo = open(BALCAO_FIFO, O_RDWR);
	if (fd_balc_fifo == -1)
	{
		printf("[ERRO] Abrir o fifo do balcao\n");
		exit(2);
	}

	//tratar thread classCli

	pthread_mutex_init(&trinco, NULL);

	balcUtData.flagContinua = 1;
	balcUtData.trinco = &trinco;

	pthread_create(&thread_classCli, NULL, threadClassCli, (void *)&balcUtData);

	while (1)
	{
		printf("\nComandos: ");
		fgets(comando, sizeof(comando), stdin);

		if (strcmp(comando, "encerra\n") == 0)
		{ //encerrar servidor com class a correr
			pthread_mutex_lock(balcUtData.trinco);
			balcUtData.flagContinua = 0;
			pthread_mutex_unlock(balcUtData.trinco);

			//para conseguir desbloquear read da thread no fifo do balcao
			write(fd_balc_fifo, &(sendExit), sizeof(char));

			printf("[AVISO] Classificador a terminar...\n");
			flag_fimClass = 1;
			break;
		}
		else
		{
			printf("Comando apenas lido, classificador esta encerrado\n");
		}
	}

	printf("[AVISO] Balcao a encerrar...\n");

	pthread_join(thread_classCli, NULL);

	pthread_mutex_destroy(&trinco);

	unlink(BALCAO_FIFO);

	exit(0);
}
