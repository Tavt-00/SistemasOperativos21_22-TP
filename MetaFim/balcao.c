#include "util.h"


//listar clientes
void lista_cli(utente_ptr array_ut, int numUt){

	printf("Existem %d utentes\n", numUt);

	for(int i = 0; i< numUt; i++)
		printf("Nome Utente: [%s] - ID Utente: [%d] - Espec atribuida: [%s] - Prioridade: [%d] \n", array_ut[i].cliente.nome, array_ut[i].cliente.pidUt, array_ut[i].cliente.espec_atr, array_ut[i].cliente.prioridade_atr);
}


//balcUtData.array_utentes, numUt, &indice, comando)
int comandos_balc (utente_ptr array_ut, int numUt, int *indice, char *comando){


	if(strcmp(comando, "utentes") == 0){
		lista_cli(array_ut, numUt);
		return 1;
	}


	return -1;
}


//alocar memória para novos utentes
utente_ptr add_ut(int *numUt, utente_ptr array_ut, user_ptr novo){

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
	strcpy(array_ut[*numUt].cliente.nome, novo->nome);
	array_ut[(*numUt)].cliente.pidUt = novo->pidUt; 
	array_ut[(*numUt)].cliente.flagRegistado = 1; //balcao registou			
	array_ut[(*numUt)].flag_emConsulta = 0;	//ainda não está em consulta
	array_ut[(*numUt)].cliente.prioridade_atr = novo->prioridade_atr;
	strcpy(array_ut[(*numUt)].cliente.espec_atr, novo->espec_atr);


	(*numUt)++;
	return array_ut;
}


// adicionar medicos
med_ptr add_med(int *numMed, med_ptr array_med, medico_ptr novo){

	if(*numMed == 0){
		array_med = malloc(sizeof(med));
		if(array_med == NULL){
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
	strcpy(array_med[*numMed].medico_esp.nome, novo->nome);
	array_med[(*numMed)].medico_esp.med_id = novo->med_id; 
	array_med[(*numMed)].medico_esp.flagRegistado = 1;
	array_med[(*numMed)].flag_emConsulta = 0;	//ainda não está em consulta

	(*numMed)++;
	return array_med;
}

//funcao para limpar todas as filas de espera
filas_ptr limpa_filas(filas_ptr arrays_filas){

	for(int i = 0; i < 5; i++){
		arrays_filas->oftalmologia[i] = NULL;
		arrays_filas->ortopedia[i] = NULL;
		arrays_filas->neurologia[i] = NULL;
		arrays_filas->estomatologia[i] = NULL;
		arrays_filas->geral[i] = NULL;
	}

	return arrays_filas;
}


//funcao para adicionar utilizador na fila respetiva
filas_ptr addUt_naFIla (filas_ptr arrays_filas, user_ptr utente){

	int contaClis = 0;

	//ver especialidade do utente
	if(strcmp(utente->espec_atr, "oftalmologia") == 0){
		
		//ver qual é o tamanho atual da fila
		//se houver espaço no array, ver prioridadees
		//se a prioridade for maior que alguma, o user que está na fila passa a 1ª pos livre. o atual entre nessa
		contaClis = 0;
		for(int i = 0; i < 5; i++){
			if(arrays_filas->oftalmologia[i] != NULL){
				contaClis ++;
			}
		}

		if(contaClis == 5)

		
	}

}

/*
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





//listar medicos
void lista_cli(med_ptr array_med, int numMed){

	printf("Existem %d medicos\n", numUt);

	for(int i = 0; i< numUt; i++)
		printf("Nome Medico: [%s] - ID Medico: [%d] - Especialidade: [%s]\n", array_med[i].medico_esp.nome, array_med[i].medico_esp.med_id, array_med[i].medico_esp.especialidade);
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
	balcUt *balcUtData = (balcUt *)dados;

	int res, prioLida, estadoFilho, x;
	int fd_cli_fifo, fd_balc_fifo, res_size;
	int balClass[2], classBalc[2];
	char class_info[256], especLida[30], fimClass[5] = "#fim\n";
	char nome_fifo_cli[30];


	utente_ptr auxUtente_ptr = balcUtData->array_utentes;
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

	//enquanto balcao não faz encerra
	while (balcUtData->flagContinua)
	{

		//le sintomas do cliente
		res_size = read(fd_balc_fifo, &cli_dados, sizeof(user));
		if (res_size != sizeof(user))
		{
			printf("[ERRO] Ler do fifo balcao\n");
			exit(3);
		}
		if(!balcUtData->flagContinua)
			break;

		pthread_mutex_lock(balcUtData->trinco);

			//se o cliente ainda não está registado, registar e falar com o classificador
		if (cli_dados.flagRegistado == 0)	
		{

			//verificar MAXCLI e guardar cliente no array
			if ((*balcUtData->numUt) < balcUtData->maxUt)
			{				
					//fazer fifo cliente
				sprintf(nome_fifo_cli, UTENTE_FIFO, cli_dados.pidUt);
				fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
				if (fd_cli_fifo == -1)
				{
					printf("[ERRO] Abrir o fifo do cliente\n");
					exit(2);
				}

				//manda sintomas para o classificador
				write(balClass[1], &(cli_dados.mensagem), strlen(cli_dados.mensagem));

				//le resposta do classificador
				x = read(classBalc[0], class_info, sizeof(class_info)-1);
				if (x == -1)
				{
					printf("[ERRO] Ler resposta do classificador");
					exit(2);
				}
				class_info[x] = '\0';
				sscanf(class_info, "%s %d", cli_dados.espec_atr, &cli_dados.prioridade_atr);

				strcpy(cli_dados.mensagem, "BALC");

				//guardar novo cliente e os seus dados
				auxUtente_ptr == add_ut(balcUtData->numUt, balcUtData->array_utentes, &cli_dados);
				balcUtData->array_utentes = auxUtente_ptr;

				//por na fila respetiva

				//responder ao cliente
				write(fd_cli_fifo, &cli_dados, sizeof(user));
				printf("O cliente %s foi registado.\n", cli_dados.nome);
				printf("Especialidade atribuida: %s. Prioridade atribuida: %d\n", cli_dados.espec_atr, cli_dados.prioridade_atr);
			}
			else
			{	
				//avisar cliente que está no maximo de clientes
				strcpy(cli_dados.mensagem, "MAXCLI");

				sprintf(nome_fifo_cli, UTENTE_FIFO, cli_dados.pidUt);
				fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
				if (fd_cli_fifo == -1)
				{
					printf("[ERRO] Abrir o fifo do balc para o cliente\n");
					exit(2);
				}
				write(fd_cli_fifo, &cli_dados, sizeof(user));
			}
		}
		else{
			//aqui deve ser para ler os comandos do cliente
			printf("BOas\n");
		}
		
		pthread_mutex_unlock(balcUtData->trinco);

	}

	write(balClass[1], fimClass, strlen(fimClass)); //avisar classificador para terminar

	wait(&estadoFilho);

	printf("FUICLIENTE\n");
	pthread_exit(NULL);
}


void *threadMedico(void *dados)		//med_ptr -> utente_ptr
{
	int fd_balc_fifo2, fd_med_fifo;
	int res_size;
	char nome_fifo_med[30];

	balcMed *balcMedData = (balcMed *) dados;

	med_ptr auxMed_ptr = balcMedData->array_medicos;
	medico med_dados;

	//tratar fifo balcao
	fd_balc_fifo2 = open(BALCAO_FIFO2, O_RDWR);
	if (fd_balc_fifo2 == -1)
	{
		printf("[ERRO] Abrir o fifo do balcao-medico\n");
		exit(2);
	}


	while(balcMedData->flagContinua){
		
		//ler dados do medico
		res_size = read (fd_balc_fifo2, &med_dados, sizeof(medico));
		if (res_size != sizeof(medico))
		{
			printf("[ERRO] Ler do fifo balcao\n");
			exit(3);
		}
		
		printf("Medico %s, espe %s", med_dados.nome, med_dados.especialidade);

		if(!balcMedData->flagContinua)
			break;

		pthread_mutex_lock(balcMedData->trinco);
		
		//se medico ainda não está registado, guardar 
		if(med_dados.flagRegistado == 0){
			
			//verificar MAXMED
			if ((*balcMedData->numMed) < balcMedData->maxMed)
			{
				sprintf(nome_fifo_med, MEDICO_FIFO, med_dados.med_id);
				fd_med_fifo = open(nome_fifo_med, O_WRONLY);
				if (fd_med_fifo == -1)
				{
					printf("[ERRO] Abrir o fifo do medico\n");
					exit(2);
				}
				printf("1\n");
				strcpy(med_dados.mensagem, "BALC");
				printf("2\n");

				//guardar o novo medico e responder
				auxMed_ptr == add_med(balcMedData->numMed, balcMedData->array_medicos, &med_dados);
				balcMedData->array_medicos = auxMed_ptr;
				printf("3\n");

				write(fd_med_fifo, &med_dados, sizeof(medico));
				printf("4\n");

			}
			else{
				//avisar medico que está no max de medicos
				strcpy(med_dados.mensagem, "MAXMED");

				sprintf(nome_fifo_med, MEDICO_FIFO, med_dados.med_id);
				fd_med_fifo = open(nome_fifo_med, O_WRONLY);
				if (fd_med_fifo == -1)
				{
					printf("[ERRO] Abrir o fifo do balc para o cliente\n");
					exit(2);
				}
				write(fd_med_fifo, &med_dados, sizeof(user));
			}
			
		}
		else{
			//aqui deve ser para ler os comandos do medico
			printf("BOas\n");
		}

		pthread_mutex_unlock(balcMedData->trinco);
	}

	printf("FUIMEDICO\n");
	pthread_exit(NULL);

}


int main(int argc, char *argv[], char *envp[])
{
	setbuf(stdout, NULL);

	int res_size, flag_fimClass = 0;
	int fd_balc_fifo, fd_balc_fifo2;
	char comando[30], sendExit = 'E';

	char *maxClientes, *maxMedicos;
	int max_clientes, max_medicos;
	int numUt = 0, numMed = 0, indice;

	utente_ptr array_ut = NULL; //ponteiro para array de clientes
	balcUt balcUtData;

	med_ptr array_med = NULL;
	balcMed balcMedData;


	pthread_t thread_classCli;
	pthread_t thread_med;
	pthread_mutex_t trinco;

	//criar fifo balcao-cliente e balcao-medico
	if (access(BALCAO_FIFO, F_OK) != 0)
	{
		mkfifo(BALCAO_FIFO, 0600);
	}
	if (access(BALCAO_FIFO2, F_OK) != 0)
	{
		mkfifo(BALCAO_FIFO2, 0600);
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

		//abrir fifo do balcao-cliente
	fd_balc_fifo = open(BALCAO_FIFO, O_RDWR);
	if (fd_balc_fifo == -1)
	{
		printf("[ERRO] Abrir o fifo do balcao-cliente\n");
		exit(2);
	}

		//abrir fifo do balcao-medico
	fd_balc_fifo2 = open(BALCAO_FIFO2, O_RDWR);
	if (fd_balc_fifo2 == -1)
	{
		printf("[ERRO] Abrir o fifo do balcao-medico\n");
		exit(2);
	}

	//preencher estrutruras das threads
	pthread_mutex_init(&trinco, NULL);

	balcUtData.flagContinua = 1;
	balcUtData.trinco = &trinco;
	balcUtData.maxUt = max_clientes;
	balcUtData.numUt = &numUt;
	balcUtData.array_utentes = array_ut;

	balcMedData.flagContinua = 1;
	balcMedData.trinco = &trinco;
	balcMedData.maxMed = max_medicos;
	balcMedData.numMed = &numMed;
	balcMedData.array_medicos = array_med;

	//thread para ouvir cliente
	pthread_create(&thread_classCli, NULL, threadClassCli, (void *) &balcUtData);
	
	//thread para ouvir medico
	pthread_create(&thread_med, NULL, threadMedico, (void*) &balcMedData);

	while (1)
	{
		printf("\nComandos: ");
		fgets(comando, sizeof(comando), stdin);

		if (strcmp(comando, "encerra\n") == 0)
		{ 	
			//parar thread balcao-cliente
			pthread_mutex_lock(balcUtData.trinco);
			balcUtData.flagContinua = 0;
			pthread_mutex_unlock(balcUtData.trinco);

			//parar thread balcao-medico
			pthread_mutex_lock(balcMedData.trinco);
			balcMedData.flagContinua = 0;
			pthread_mutex_unlock(balcMedData.trinco);

			//desbloquear read
			write(fd_balc_fifo, &(sendExit), sizeof(char));
			write(fd_balc_fifo2, &(sendExit), sizeof(char));

			printf("[AVISO] Classificador a terminar...\n");
			flag_fimClass = 1;
			break;
		}
		
		int op = comandos_balc(balcUtData.array_utentes, numUt, &indice, comando);

		if (op == 1);
		else if(op == 2){
			printf("em processamento..\n");
		}

	}

	printf("[AVISO] Balcao a encerrar...\n");

	pthread_join(thread_classCli, NULL);	
	pthread_join(thread_med, NULL);

	pthread_mutex_destroy(&trinco);

	unlink(BALCAO_FIFO);

	exit(0);
}
