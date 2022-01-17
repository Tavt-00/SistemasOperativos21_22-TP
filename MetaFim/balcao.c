#include "util.h"

utente_ptr array_ut = NULL;
med_ptr array_med = NULL;
filas Filas;


//alocar memória para novos utentes
utente_ptr add_ut(int *numUt, user_ptr novo){

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
	strcpy(array_ut[(*numUt)].cliente.espec_atr, novo->espec_atr);
	array_ut[(*numUt)].cliente.pidUt = novo->pidUt; 
	array_ut[(*numUt)].cliente.flagRegistado = 1; //balcao registou			
	array_ut[(*numUt)].cliente.flagEmConsulta = 0;	//ainda não está em consulta
	array_ut[(*numUt)].cliente.flagNaFila = 1; //se veio para aqui é porque esta na fila
	array_ut[(*numUt)].cliente.prioridade_atr = novo->prioridade_atr;

	(*numUt)++;

	return array_ut;
}


// adicionar medicos
med_ptr add_med(int *numMed, medico_ptr novo){

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
	strcpy(array_med[*numMed].medico_esp.especialidade, novo->especialidade);
	array_med[(*numMed)].medico_esp.med_id = novo->med_id; 
	array_med[(*numMed)].medico_esp.flagRegistado = 1;
	array_med[(*numMed)].medico_esp.flagEmConsulta = 0;	//ainda não está em consulta

	(*numMed)++;
	return array_med;
}


//remover cliente do array dinâmico
utente_ptr remover_ut(int *numUt, int pid){

	utente_ptr aux;

	if(*numUt > 1){

		int i = 0;

		//encontrar o client a tirar e guardar posicao
		while(array_ut[i].cliente.pidUt != pid){
			i++;
		}

		if(i != *numUt-1)
			array_ut[i] = array_ut[*numUt-1];

		utente_ptr aux;
		aux = realloc(array_ut, sizeof(utente)*(*numUt-1));
		if (aux == NULL)
		{
			printf("[ERRO] Realocacao de memoria - func remover_ut\n");
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

void encerra_todos(int numMed, int numUt ){

	char fifoMed[50], fifoCli[50];
	int fd_cli, fd_med, auxPid;
	medico med_dados;
	user cli_dados;

	//acabar com medicos
	for(int i = 0; i < numMed; i++){
		auxPid = array_med[i].medico_esp.med_id;

		strcpy(array_med[i].medico_esp.mensagem, "ENCERRA");
		sprintf(fifoMed, MEDICO_FIFO, auxPid);

		fd_med = open(fifoMed, O_WRONLY);
		write(fd_med, &array_med[i].medico_esp, sizeof(medico));
		array_med[i].medico_esp.flagRegistado = 0;
	}

	//acabar com utentes
	for(int i = 0; i < numUt; i++){
		auxPid = array_ut[i].cliente.pidUt;

		strcpy(array_ut[i].cliente.mensagem, "ENCERRA");
		sprintf(fifoCli, UTENTE_FIFO, auxPid);

		fd_cli = open(fifoCli, O_WRONLY);
		write(fd_cli, &array_ut[i].cliente, sizeof(user));
		array_ut->cliente.flagRegistado = 0;
	}
}

// verificiar se é possível adicionar cliente na sua fila
void check_filaOftal ( user novo, int *numUt , int *resultado){

	//resultado: se -1 é porque não foi adicionado à fila de espera | >= 0 indica quantas pessoas tem à sua frente
	int contaClis = 0, flagEncontrou = 0, fd_cli_fifo;
	char nome_fifo_cli[30];

	
	//FILA OFTALMOLOGIA
	if (strcmp(novo.espec_atr, "oftalmologia") == 0)
	{
		contaClis = 0;

		for (int i = 0; i < 5; i++)
		{
			if (Filas.oftalmologia[i].flagNaFila != -1)
				contaClis++;
		}

		//se fila vazia, meter logo na 1ª pos
		if (contaClis == 0){
			novo.flagNaFila = 1;
			Filas.oftalmologia[0] = novo;
			*resultado = 0;	//0 pessoas à frente
		}


		// se fila cheia
		else if(contaClis == 5){
			//ver se há alguém com prioridade menor
			for (int i = 0; i < 5; i++)
			{	
				if (Filas.oftalmologia[i].prioridade_atr > novo.prioridade_atr)
				{	//se encontrou, ver em que posição foi
	
					//se foi na ultima, só temos de mandar embora o ultimo, tirá-lo do array geral e colocar o novo na fila
					if(i == 4){
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.oftalmologia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						//avisar cliente que vai ser substituído
						printf("Aqui1\n");
						strcpy(Filas.oftalmologia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.oftalmologia[4], sizeof(user));

						//tirar do array geral 
						array_ut = remover_ut(numUt, Filas.oftalmologia[4].pidUt);

						//colocar novo utente na fila e pos respetiva
						novo.flagNaFila = 1;
						Filas.oftalmologia[4] = novo;
						flagEncontrou = 1;
						*resultado = i; //i tb vai ser igual ao numero de utentes à frente pois começa em 0
						break; 
					}

					//se não foi na última, por exemp i = 1
					else{
						int aux = 3;

						//o ultimo vai ser sacrificado pois a fila esta cheia
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.oftalmologia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO2] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						strcpy(Filas.oftalmologia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.oftalmologia[4], sizeof(user));
						array_ut = remover_ut(numUt, Filas.oftalmologia[4].pidUt);
						
						//andar todos com uma posição a partir do lugar encontrado
						for (int j = aux; j >= i; j--)
						{
							Filas.oftalmologia[j+1] = Filas.oftalmologia[j];
						}
						
						novo.flagNaFila = 1;
						Filas.oftalmologia[i] = novo;
						flagEncontrou = 1;
						*resultado = i;
						break;
					}
				}
			}
			//se não encontrou vaga
			if (flagEncontrou == 0){
				*resultado = -1; 
			}	
		}
		//se ainda houver espaço na fila, só preciso de andar com os outros, meter o novo e não tirar ninguém
		else if (contaClis > 0 && contaClis < 5) 
		{
			for (int i = 0; i < 5; i++)
			{
				if (Filas.oftalmologia[i].prioridade_atr > novo.prioridade_atr)
				{
					int aux = 4;
					
					//encontrar a 1ª pos ocupada a contar da dirt para esq
					while (1)
					{	
						if (Filas.oftalmologia[aux].flagNaFila == -1)
						{
							aux--;
						}
						else
							break;
					}
					//aux sai daqui com a pos certa onde é preciso começar a andar para a direita

					for (int j = aux; j >= i; j--)
					{
						Filas.oftalmologia[j+1] = Filas.oftalmologia[j];
					}
					
					novo.flagNaFila = 1;
					Filas.oftalmologia[i] = novo;
					flagEncontrou = 1;
					*resultado = i;
					break;
				} 
			}
			//se não encontrou ninguém com prio menor, vai para a vaga disponível  
			if (flagEncontrou == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (Filas.oftalmologia[i].flagNaFila == -1){
						novo.flagNaFila = 1;
						Filas.oftalmologia[i] = novo;
						*resultado = i;
						break;
					}	
				}
			}
		}
	}
}


void check_filaNeu (user novo, int *numUt, int *resultado){

	int contaClis = 0, flagEncontrou = 0, fd_cli_fifo;
	char nome_fifo_cli[30];

	//FILA NEUROLOGIA
	if (strcmp(novo.espec_atr, "neurologia") == 0)
	{
		contaClis = 0;

		for (int i = 0; i < 5; i++)
		{
			if (Filas.neurologia[i].flagNaFila != -1)
				contaClis++;
		}

		//se fila vazia, meter logo na 1ª pos
		if (contaClis == 0){
			novo.flagNaFila = 1;
			Filas.neurologia[0] = novo;
			*resultado = 0;	//0 pessoas à frente
		}


		// se fila cheia
		else if(contaClis == 5){
			//ver se há alguém com prioridade menor
			for (int i = 0; i < 5; i++)
			{	
				if (Filas.neurologia[i].prioridade_atr > novo.prioridade_atr)
				{	//se encontrou, ver em que posição foi
	
					//se foi na ultima, só temos de mandar embora o ultimo, tirá-lo do array geral e colocar o novo na fila
					if(i == 4){
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.neurologia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						//avisar cliente que vai ser substituído
						printf("Aqui1\n");
						strcpy(Filas.neurologia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.neurologia[4], sizeof(user));

						//tirar do array geral 
						array_ut = remover_ut(numUt, Filas.neurologia[4].pidUt);

						//colocar novo utente na fila e pos respetiva
						novo.flagNaFila = 1;
						Filas.neurologia[4] = novo;
						flagEncontrou = 1;
						*resultado = i; //i tb vai ser igual ao numero de utentes à frente pois começa em 0
						break; 
					}

					//se não foi na última, por exemp i = 1
					else{
						int aux = 3;

						//o ultimo vai ser sacrificado pois a fila esta cheia
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.neurologia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO2] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						strcpy(Filas.neurologia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.neurologia[4], sizeof(user));
						array_ut = remover_ut(numUt, Filas.neurologia[4].pidUt);
						
						//andar todos com uma posição a partir do lugar encontrado
						for (int j = aux; j >= i; j--)
						{
							Filas.neurologia[j+1] = Filas.neurologia[j];
						}
						
						novo.flagNaFila = 1;
						Filas.neurologia[i] = novo;
						flagEncontrou = 1;
						*resultado = i;
						break;
					}
				}
			}
			//se não encontrou vaga
			if (flagEncontrou == 0){
				*resultado = -1; 
			}	
		}
		//se ainda houver espaço na fila, só preciso de andar com os outros, meter o novo e não tirar ninguém
		else if (contaClis > 0 && contaClis < 5) 
		{
			for (int i = 0; i < 5; i++)
			{
				if (Filas.neurologia[i].prioridade_atr > novo.prioridade_atr)
				{
					int aux = 4;
					
					//encontrar a 1ª pos ocupada a contar da dirt para esq
					while (1)
					{	
						if (Filas.neurologia[aux].flagNaFila == -1)
						{
							aux--;
						}
						else
							break;
					}
					//aux sai daqui com a pos certa onde é preciso começar a andar para a direita

					for (int j = aux; j >= i; j--)
					{
						Filas.neurologia[j+1] = Filas.neurologia[j];
					}
					
					novo.flagNaFila = 1;
					Filas.neurologia[i] = novo;
					flagEncontrou = 1;
					*resultado = i;
					break;
				} 
			}
			//se não encontrou ninguém com prio menor, vai para a vaga disponível  
			if (flagEncontrou == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (Filas.neurologia[i].flagNaFila == -1){
						novo.flagNaFila = 1;
						Filas.neurologia[i] = novo;
						*resultado = i;
						break;
					}	
				}
			}
		}
	}
}

	
void check_filaEsto (user novo, int *numUt , int *resultado){

	int contaClis = 0, flagEncontrou = 0, fd_cli_fifo;
	char nome_fifo_cli[30];


	//FILA ESTOMATOLOGIA
	if (strcmp(novo.espec_atr, "estomatologia") == 0)
	{
		contaClis = 0;

		for (int i = 0; i < 5; i++)
		{
			if (Filas.estomatologia[i].flagNaFila != -1)
				contaClis++;
		}

		//se fila vazia, meter logo na 1ª pos
		if (contaClis == 0){
			novo.flagNaFila = 1;
			Filas.estomatologia[0] = novo;
			*resultado = 0;	//0 pessoas à frente
		}


		// se fila cheia
		else if(contaClis == 5){
			//ver se há alguém com prioridade menor
			for (int i = 0; i < 5; i++)
			{	
				if (Filas.estomatologia[i].prioridade_atr > novo.prioridade_atr)
				{	//se encontrou, ver em que posição foi
	
					//se foi na ultima, só temos de mandar embora o ultimo, tirá-lo do array geral e colocar o novo na fila
					if(i == 4){
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.estomatologia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						//avisar cliente que vai ser substituído
						printf("Aqui1\n");
						strcpy(Filas.estomatologia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.estomatologia[4], sizeof(user));

						//tirar do array geral 
						array_ut = remover_ut(numUt, Filas.estomatologia[4].pidUt);

						//colocar novo utente na fila e pos respetiva
						novo.flagNaFila = 1;
						Filas.estomatologia[4] = novo;
						flagEncontrou = 1;
						*resultado = i; //i tb vai ser igual ao numero de utentes à frente pois começa em 0
						break; 
					}

					//se não foi na última, por exemp i = 1
					else{
						int aux = 3;

						//o ultimo vai ser sacrificado pois a fila esta cheia
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.estomatologia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO2] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						strcpy(Filas.estomatologia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.estomatologia[4], sizeof(user));
						array_ut = remover_ut(numUt, Filas.estomatologia[4].pidUt);
						
						//andar todos com uma posição a partir do lugar encontrado
						for (int j = aux; j >= i; j--)
						{
							Filas.estomatologia[j+1] = Filas.estomatologia[j];
						}
						
						novo.flagNaFila = 1;
						Filas.estomatologia[i] = novo;
						flagEncontrou = 1;
						*resultado = i;
						break;
					}
				}
			}
			//se não encontrou vaga
			if (flagEncontrou == 0){
				*resultado = -1; 
			}	
		}
		//se ainda houver espaço na fila, só preciso de andar com os outros, meter o novo e não tirar ninguém
		else if (contaClis > 0 && contaClis < 5) 
		{
			for (int i = 0; i < 5; i++)
			{
				if (Filas.estomatologia[i].prioridade_atr > novo.prioridade_atr)
				{
					int aux = 4;
					
					//encontrar a 1ª pos ocupada a contar da dirt para esq
					while (1)
					{	
						if (Filas.estomatologia[aux].flagNaFila == -1)
						{
							aux--;
						}
						else
							break;
					}
					//aux sai daqui com a pos certa onde é preciso começar a andar para a direita

					for (int j = aux; j >= i; j--)
					{
						Filas.estomatologia[j+1] = Filas.estomatologia[j];
					}
					
					novo.flagNaFila = 1;
					Filas.estomatologia[i] = novo;
					flagEncontrou = 1;
					*resultado = i;
					break;
				} 
			}
			//se não encontrou ninguém com prio menor, vai para a vaga disponível  
			if (flagEncontrou == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (Filas.estomatologia[i].flagNaFila == -1){
						novo.flagNaFila = 1;
						Filas.estomatologia[i] = novo;
						*resultado = i;
						break;
					}	
				}
			}
		}
	}
}


void check_filaOrto (user novo, int *numUt , int *resultado){

	int contaClis = 0, flagEncontrou = 0, fd_cli_fifo;
	char nome_fifo_cli[30];

	//FILA ORTOPEDIA
	if (strcmp(novo.espec_atr, "ortopedia") == 0)
	{
		contaClis = 0;

		for (int i = 0; i < 5; i++)
		{
			if (Filas.ortopedia[i].flagNaFila != -1)
				contaClis++;
		}

		//se fila vazia, meter logo na 1ª pos
		if (contaClis == 0){
			novo.flagNaFila = 1;
			Filas.ortopedia[0] = novo;
			*resultado = 0;	//0 pessoas à frente
		}


		// se fila cheia
		else if(contaClis == 5){
			//ver se há alguém com prioridade menor
			for (int i = 0; i < 5; i++)
			{	
				if (Filas.ortopedia[i].prioridade_atr > novo.prioridade_atr)
				{	//se encontrou, ver em que posição foi
	
					//se foi na ultima, só temos de mandar embora o ultimo, tirá-lo do array geral e colocar o novo na fila
					if(i == 4){
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.ortopedia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						//avisar cliente que vai ser substituído
						printf("Aqui1\n");
						strcpy(Filas.ortopedia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.ortopedia[4], sizeof(user));

						//tirar do array geral 
						array_ut = remover_ut(numUt, Filas.ortopedia[4].pidUt);

						//colocar novo utente na fila e pos respetiva
						novo.flagNaFila = 1;
						Filas.ortopedia[4] = novo;
						flagEncontrou = 1;
						*resultado = i; //i tb vai ser igual ao numero de utentes à frente pois começa em 0
						break; 
					}

					//se não foi na última, por exemp i = 1
					else{
						int aux = 3;

						//o ultimo vai ser sacrificado pois a fila esta cheia
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.ortopedia[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO2] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						strcpy(Filas.ortopedia[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.ortopedia[4], sizeof(user));
						array_ut = remover_ut(numUt, Filas.ortopedia[4].pidUt);
						
						//andar todos com uma posição a partir do lugar encontrado
						for (int j = aux; j >= i; j--)
						{
							Filas.ortopedia[j+1] = Filas.ortopedia[j];
						}
						
						novo.flagNaFila = 1;
						Filas.ortopedia[i] = novo;
						flagEncontrou = 1;
						*resultado = i;
						break;
					}
				}
			}
			//se não encontrou vaga
			if (flagEncontrou == 0){
				*resultado = -1; 
			}	
		}
		//se ainda houver espaço na fila, só preciso de andar com os outros, meter o novo e não tirar ninguém
		else if (contaClis > 0 && contaClis < 5) 
		{
			for (int i = 0; i < 5; i++)
			{
				if (Filas.ortopedia[i].prioridade_atr > novo.prioridade_atr)
				{
					int aux = 4;
					
					//encontrar a 1ª pos ocupada a contar da dirt para esq
					while (1)
					{	
						if (Filas.ortopedia[aux].flagNaFila == -1)
						{
							aux--;
						}
						else
							break;
					}
					//aux sai daqui com a pos certa onde é preciso começar a andar para a direita

					for (int j = aux; j >= i; j--)
					{
						Filas.ortopedia[j+1] = Filas.ortopedia[j];
					}
					
					novo.flagNaFila = 1;
					Filas.ortopedia[i] = novo;
					flagEncontrou = 1;
					*resultado = i;
					break;
				} 
			}
			//se não encontrou ninguém com prio menor, vai para a vaga disponível  
			if (flagEncontrou == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (Filas.ortopedia[i].flagNaFila == -1){
						novo.flagNaFila = 1;
						Filas.ortopedia[i] = novo;
						*resultado = i;
						break;
					}	
				}
			}
		}
	}
}


void check_filaGe (user novo, int *numUt , int *resultado){

	int contaClis = 0, flagEncontrou = 0, fd_cli_fifo;
	char nome_fifo_cli[30];

	//FILA GERAL
	if (strcmp(novo.espec_atr, "geral") == 0)
	{
		contaClis = 0;

		for (int i = 0; i < 5; i++)
		{
			if (Filas.geral[i].flagNaFila != -1)
				contaClis++;
		}

		//se fila vazia, meter logo na 1ª pos
		if (contaClis == 0){
			novo.flagNaFila = 1;
			Filas.geral[0] = novo;
			*resultado = 0;	//0 pessoas à frente
		}


		// se fila cheia
		else if(contaClis == 5){
			//ver se há alguém com prioridade menor
			for (int i = 0; i < 5; i++)
			{	
				if (Filas.geral[i].prioridade_atr > novo.prioridade_atr)
				{	//se encontrou, ver em que posição foi
	
					//se foi na ultima, só temos de mandar embora o ultimo, tirá-lo do array geral e colocar o novo na fila
					if(i == 4){
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.geral[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						//avisar cliente que vai ser substituído
						printf("Aqui1\n");
						strcpy(Filas.geral[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.geral[4], sizeof(user));

						//tirar do array geral 
						array_ut = remover_ut(numUt, Filas.geral[4].pidUt);

						//colocar novo utente na fila e pos respetiva
						novo.flagNaFila = 1;
						Filas.geral[4] = novo;
						flagEncontrou = 1;
						*resultado = i; //i tb vai ser igual ao numero de utentes à frente pois começa em 0
						break; 
					}

					//se não foi na última, por exemp i = 1
					else{
						int aux = 3;

						//o ultimo vai ser sacrificado pois a fila esta cheia
						sprintf(nome_fifo_cli, UTENTE_FIFO, Filas.geral[4].pidUt);
						fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
						if (fd_cli_fifo == -1)
						{
							printf("[ERRO2] Abrir o fifo do cliente - func check_fila\n");
							exit(2);
						}
						strcpy(Filas.geral[4].mensagem, "BALC_FILA_NOMORE_OK");
						write(fd_cli_fifo, &Filas.geral[4], sizeof(user));
						array_ut = remover_ut(numUt, Filas.geral[4].pidUt);
						
						//andar todos com uma posição a partir do lugar encontrado
						for (int j = aux; j >= i; j--)
						{
							Filas.geral[j+1] = Filas.geral[j];
						}
						
						novo.flagNaFila = 1;
						Filas.geral[i] = novo;
						flagEncontrou = 1;
						*resultado = i;
						break;
					}
				}
			}
			//se não encontrou vaga
			if (flagEncontrou == 0){
				*resultado = -1; 
			}	
		}
		//se ainda houver espaço na fila, só preciso de andar com os outros, meter o novo e não tirar ninguém
		else if (contaClis > 0 && contaClis < 5) 
		{
			for (int i = 0; i < 5; i++)
			{
				if (Filas.geral[i].prioridade_atr > novo.prioridade_atr)
				{
					int aux = 4;
					
					//encontrar a 1ª pos ocupada a contar da dirt para esq
					while (1)
					{	
						if (Filas.geral[aux].flagNaFila == -1)
						{
							aux--;
						}
						else
							break;
					}
					//aux sai daqui com a pos certa onde é preciso começar a andar para a direita

					for (int j = aux; j >= i; j--)
					{
						Filas.geral[j+1] = Filas.geral[j];
					}
					
					novo.flagNaFila = 1;
					Filas.geral[i] = novo;
					flagEncontrou = 1;
					*resultado = i;
					break;
				} 
			}
			//se não encontrou ninguém com prio menor, vai para a vaga disponível  
			if (flagEncontrou == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (Filas.geral[i].flagNaFila == -1){
						novo.flagNaFila = 1;
						Filas.geral[i] = novo;
						*resultado = i;
						break;
					}	
				}
			}
		}
	}

}


void arranja_filas (int pid, char espec[20]){


	if(strcmp(espec, "oftalmologia") == 0){

		int contaClis = 0, pos = -1, i;

		//procurar pelo pid a tirar e guardar posição
		for(i = 0; i<5; i++){
			if(Filas.oftalmologia[i].flagNaFila == 1){
				if(Filas.oftalmologia[i].pidUt == pid){
					pos = i;
				}
			}
		}
		int aux = pos + 1;
		
		//ver ocupação da fila
		for(int i = 0; i<5; i++){
			if(Filas.oftalmologia[i].flagNaFila == 1){
				contaClis++;
			}
		}

		if(contaClis == 5){
			for(int j = aux; j < 5; j++){
				Filas.oftalmologia[j-1] = Filas.oftalmologia[j];
			}

			//se a fila está cheia, vai ser a última posição que fica livre
			Filas.oftalmologia[4].flagNaFila = -1;
		}
		else if(contaClis > 1 && contaClis < 5){
			int limpa = contaClis - 1; 

			for(int j = aux; j <= limpa; j++){
				Filas.oftalmologia[j-1] = Filas.oftalmologia[j];
			}

			//limpar a posição que deve ficar livre
			Filas.oftalmologia[limpa].flagNaFila = -1;
		}
		
		//se só houver um na fila, so é preciso tirar esse
		else if(contaClis == 1){
			Filas.oftalmologia[0].flagNaFila = -1;
		}
	}

	if(strcmp(espec, "neurologia") == 0){

		int contaClis = 0, pos = -1, i;

		//procurar pelo pid a tirar e guardar posição
		for(i = 0; i<5; i++){
			if(Filas.neurologia[i].flagNaFila == 1){
				if(Filas.neurologia[i].pidUt == pid){
					pos = i;
				}
			}
		}
		int aux = i + 1;
		
		//ver ocupação da fila
		for(int i = 0; i<5; i++){
			if(Filas.neurologia[i].flagNaFila == 1){
				contaClis++;
			}
		}

		if(contaClis == 5){
			for(int j = aux; j < 5; j++){
				Filas.neurologia[j] = Filas.neurologia[j-1];
			}

			//se a fila está cheia, vai ser a última posição que fica livre
			Filas.neurologia[4].flagNaFila = -1;
		}
		else if(contaClis > 1 && contaClis < 5){
			int limpa = contaClis - 1; 

			for(int j = aux; j <= limpa; j++){
				Filas.neurologia[j] = Filas.neurologia[j-1];
			}

			//limpar a posição que deve ficar livre
			Filas.neurologia[limpa].flagNaFila = -1;
		}
		
		//se só houver um na fila, so é preciso tirar esse
		else if(contaClis == 1){
			Filas.neurologia[0].flagNaFila = -1;
		}
	}


	if(strcmp(espec, "estomalogia") == 0){

		int contaClis = 0, pos = -1, i;

		//procurar pelo pid a tirar e guardar posição
		for(i = 0; i<5; i++){
			if(Filas.estomatologia[i].flagNaFila == 1){
				if(Filas.estomatologia[i].pidUt == pid){
					pos = i;
				}
			}
		}
		int aux = i + 1;
		
		//ver ocupação da fila
		for(int i = 0; i<5; i++){
			if(Filas.estomatologia[i].flagNaFila == 1){
				contaClis++;
			}
		}

		if(contaClis == 5){
			for(int j = aux; j < 5; j++){
				Filas.estomatologia[j] = Filas.estomatologia[j-1];
			}

			//se a fila está cheia, vai ser a última posição que fica livre
			Filas.estomatologia[4].flagNaFila = -1;
		}
		else if(contaClis > 1 && contaClis < 5){
			int limpa = contaClis - 1; 

			for(int j = aux; j <= limpa; j++){
				Filas.estomatologia[j] = Filas.estomatologia[j-1];
			}

			//limpar a posição que deve ficar livre
			Filas.estomatologia[limpa].flagNaFila = -1;
		}
		
		//se só houver um na fila, so é preciso tirar esse
		else if(contaClis == 1){
			Filas.estomatologia[0].flagNaFila = -1;
		}
	}


	if(strcmp(espec, "ortopedia") == 0){

		int contaClis = 0, pos = -1, i;

		//procurar pelo pid a tirar e guardar posição
		for(i = 0; i<5; i++){
			if(Filas.ortopedia[i].flagNaFila == 1){
				if(Filas.ortopedia[i].pidUt == pid){
					pos = i;
				}
			}
		}
		int aux = i + 1;
		
		//ver ocupação da fila
		for(int i = 0; i<5; i++){
			if(Filas.ortopedia[i].flagNaFila == 1){
				contaClis++;
			}
		}

		if(contaClis == 5){
			for(int j = aux; j < 5; j++){
				Filas.ortopedia[j] = Filas.ortopedia[j-1];
			}

			//se a fila está cheia, vai ser a última posição que fica livre
			Filas.ortopedia[4].flagNaFila = -1;
		}
		else if(contaClis > 1 && contaClis < 5){
			int limpa = contaClis - 1; 

			for(int j = aux; j <= limpa; j++){
				Filas.ortopedia[j] = Filas.ortopedia[j-1];
			}

			//limpar a posição que deve ficar livre
			Filas.ortopedia[limpa].flagNaFila = -1;
		}
		
		//se só houver um na fila, so é preciso tirar esse
		else if(contaClis == 1){
			Filas.ortopedia[0].flagNaFila = -1;
		}
	}

	if(strcmp(espec, "geral") == 0){

		int contaClis = 0, pos = -1, i;

		//procurar pelo pid a tirar e guardar posição
		for(i = 0; i<5; i++){
			if(Filas.geral[i].flagNaFila == 1){
				if(Filas.geral[i].pidUt == pid){
					pos = i;
				}
			}
		}
		int aux = i + 1;
		
		//ver ocupação da fila
		for(int i = 0; i<5; i++){
			if(Filas.geral[i].flagNaFila == 1){
				contaClis++;
			}
		}

		if(contaClis == 5){
			for(int j = aux; j < 5; j++){
				Filas.geral[j] = Filas.geral[j-1];
			}

			//se a fila está cheia, vai ser a última posição que fica livre
			Filas.geral[4].flagNaFila = -1;
		}
		else if(contaClis > 1 && contaClis < 5){
			int limpa = contaClis - 1; 

			for(int j = aux; j <= limpa; j++){
				Filas.geral[j] = Filas.geral[j-1];
			}

			//limpar a posição que deve ficar livre
			Filas.geral[limpa].flagNaFila = -1;
		}
		
		//se só houver um na fila, so é preciso tirar esse
		else if(contaClis == 1){
			Filas.geral[0].flagNaFila = -1;
		}
	}

}



//listar todos os clientes, atendidos e em fila
void listaUtentes(utente_ptr array_ut, int numUt){

	printf("\n----Existem %d utentes no total----\n\n", numUt);

	for(int i = 0; i< numUt; i++){
		printf("\nNome Utente: [%s] - ID Utente: [%d] - Espec atribuida: [%s] - Prioridade: [%d] \n", array_ut[i].cliente.nome, array_ut[i].cliente.pidUt, array_ut[i].cliente.espec_atr, array_ut[i].cliente.prioridade_atr);
		if (array_ut[i].cliente.flagEmConsulta == 0)
			printf("O utente ainda se encontra na fila de espera.\n");
		else if(array_ut[i].cliente.flagEmConsulta == 1)
			printf("O utente encontra-se a ser atendido pelo especilialista: %s.\n", array_ut[i].cliente.medico_atr);	
	}
}




//listar medicos
void lista_med(med_ptr array_med, int numMed){

	printf("\n\n----Existem %d medicos no total----\n\n", numMed);

	for(int i = 0; i< numMed; i++){
		printf("\nNome Medico: [%s] - ID Medico: [%d] - Especialidade: [%s]\n", array_med[i].medico_esp.nome, array_med[i].medico_esp.med_id, array_med[i].medico_esp.especialidade);
		if(array_med[i].medico_esp.flagEmConsulta == 0)
			printf("O medico ainda nao esta em consulta com ninguem\n");
		else if(array_med[i].medico_esp.flagEmConsulta == 1)
			printf("O medico esta a consultar o utente %s.\n", array_med[i].medico_esp.cliente_atual.nome);
	}
}

void lista_filas()
{
	int contaOf = 0, contaNeu = 0, contaGe = 0, contaEst = 0, contaOrt = 0;

	printf("\n\n---Ocupacao das filas---\n");

	while (Filas.oftalmologia[contaOf].flagNaFila != -1)
	{
		contaOf++;
	}
	while (Filas.neurologia[contaNeu].flagNaFila != -1)
	{
		contaNeu++;
	}
	while (Filas.geral[contaGe].flagNaFila != -1)
	{
		contaGe++;
	}
	while (Filas.estomatologia[contaEst].flagNaFila != -1)
	{
		contaEst++;
	}
	while (Filas.ortopedia[contaOrt].flagNaFila != -1)
	{
		contaOrt++;
	}

	printf("Oftalmologia: [%d] - Neurologia [%d] - Estomatologia [%d] - Ortopedia [%d] - Geral [%d]\n\n", contaOf, contaNeu, contaEst, contaOrt, contaGe);

}

//remover medicos 
med_ptr remover_med(int *numMed, int pid){


	med_ptr aux;

	if(*numMed > 1){

		int i = 0;

		//encontrar o client a tirar e guardar posicao
		while(array_med[i].medico_esp.med_id != pid){
			i++;
		}

		if(i != *numMed-1)
			array_med[i] = array_med[*numMed-1];

		med_ptr aux;
		aux = realloc(array_med, sizeof(med)*(*numMed-1));
		if (aux == NULL)
		{
			printf("[ERRO] Realocacao de memoria - func remover_ut\n");
			free(aux);
			return NULL;
		}
		
		array_med = aux;
		(*numMed)--;

		return array_med;
	}	
	else if (*numMed == 1){
		free (array_ut);
		*numMed = 0;
		return NULL;
	}

}


void delutX (char nomeRemo[30], int *numUt){

	char espec[30], nome_fifo_cli[30];
	int pid, fd_cli_fifo, encontrou = 0;
	user cli_dados;

	//procurar cliente, saber o pid e a especialidade
	for(int i = 0; i < *numUt; i++){
		if(strcmp(array_ut[i].cliente.nome, nomeRemo) == 0){
			if(array_ut[i].cliente.flagEmConsulta == 1){
				printf("\n\n[AVISO] O cliente encontra-se em consulta!\n\n");
				return;
			}
			strcpy(espec, array_ut[i].cliente.espec_atr);
			pid = array_ut[i].cliente.pidUt;
			encontrou = 1;
		}
	}

	if(encontrou == 0){
		printf("\n\n[AVISO] O cliente nao existe!\n\n");
		return;
	}

	array_ut = remover_ut(numUt, pid);


	if(strcmp(espec, "oftalmologia") == 0){
		for (int i = 0; i < 5; i++)
		{
			if(Filas.oftalmologia[i].pidUt == pid){
				Filas.oftalmologia[i].flagNaFila = -1;
			}
		}

		arranja_filas(pid, espec);
		
		//avisar cliente
		sprintf(nome_fifo_cli, UTENTE_FIFO, pid);
		fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
		if (fd_cli_fifo == -1)
		{
			printf("[ERRO] Abrir o fifo do cliente\n");
			exit(2);
		}
		strcpy(cli_dados.mensagem, "DELEUT");
		write(fd_cli_fifo, &cli_dados, sizeof(user));

	}
	else if(strcmp(espec, "neurologia") == 0){
		for (int i = 0; i < 5; i++)
		{
			if(Filas.neurologia[i].pidUt == pid){
				Filas.neurologia[i].flagNaFila = -1;
			}
		}

		arranja_filas(pid, espec);
		
		//avisar cliente
		sprintf(nome_fifo_cli, UTENTE_FIFO, pid);
		fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
		if (fd_cli_fifo == -1)
		{
			printf("[ERRO] Abrir o fifo do cliente\n");
			exit(2);
		}
		strcpy(cli_dados.mensagem, "DELEUT");
		write(fd_cli_fifo, &cli_dados, sizeof(user));

	}
	else if(strcmp(espec, "estomatologia") == 0){
		for (int i = 0; i < 5; i++)
		{
			if(Filas.estomatologia[i].pidUt == pid){
				Filas.estomatologia[i].flagNaFila = -1;
			}
		}

		arranja_filas(pid, espec);
		
		//avisar cliente
		sprintf(nome_fifo_cli, UTENTE_FIFO, pid);
		fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
		if (fd_cli_fifo == -1)
		{
			printf("[ERRO] Abrir o fifo do cliente\n");
			exit(2);
		}
		strcpy(cli_dados.mensagem, "DELEUT");
		write(fd_cli_fifo, &cli_dados, sizeof(user));

	}
	else if(strcmp(espec, "ortopedia") == 0){
		for (int i = 0; i < 5; i++)
		{
			if(Filas.ortopedia[i].pidUt == pid){
				Filas.ortopedia[i].flagNaFila = -1;
			}
		}

		arranja_filas(pid, espec);
		
		//avisar cliente
		sprintf(nome_fifo_cli, UTENTE_FIFO, pid);
		fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
		if (fd_cli_fifo == -1)
		{
			printf("[ERRO] Abrir o fifo do cliente\n");
			exit(2);
		}
		strcpy(cli_dados.mensagem, "DELEUT");
		write(fd_cli_fifo, &cli_dados, sizeof(user));

	}
	else if(strcmp(espec, "geral") == 0){
		for (int i = 0; i < 5; i++)
		{
			if(Filas.geral[i].pidUt == pid){
				Filas.geral[i].flagNaFila = -1;
			}
		}

		arranja_filas(pid, espec);
		
		//avisar cliente
		sprintf(nome_fifo_cli, UTENTE_FIFO, pid);
		fd_cli_fifo = open(nome_fifo_cli, O_WRONLY);
		if (fd_cli_fifo == -1)
		{
			printf("[ERRO] Abrir o fifo do cliente\n");
			exit(2);
		}
		strcpy(cli_dados.mensagem, "DELEUT");
		write(fd_cli_fifo, &cli_dados, sizeof(user));

	}
}


void delespX (char nomeRemo[30], int *numMed){
	char  nome_fifo_med[30];
	int pid, fd_med_fifo, encontrou = 0;
	medico med_dados;

		//procurar cliente, saber o pid e a especialidade
	for(int i = 0; i < *numMed; i++){
		if(strcmp(array_med[i].medico_esp.nome, nomeRemo) == 0){
			if(array_med[i].medico_esp.flagEmConsulta == 1){
				printf("\n\n[AVISO] O especialistas encontra-se em consulta!\n\n");
				return;
			}
			pid = array_med[i].medico_esp.med_id;
			encontrou = 1;
		}
	}

	if (encontrou == 0)
	{
		printf("\n\n[AVISO] O medico nao existe!\n\n");
		return;
	}

	array_med = remover_med(numMed, pid);

	//avisar medico
	sprintf(nome_fifo_med, MEDICO_FIFO, pid);
	fd_med_fifo = open(nome_fifo_med, O_WRONLY);
	if (fd_med_fifo == -1)
	{
		printf("[ERRO] Abrir o fifo do cliente\n");
		exit(2);
	}
	strcpy(med_dados.mensagem, "DELESP");
	write(fd_med_fifo, &med_dados, sizeof(medico));
}


void *threadFreqN(void *dados){

	freqN *freqData = (freqN*) dados;


	while (freqData->flagContinua)
	{
		sleep(freqData->segundos);
		lista_filas();	
	}

	pthread_exit(NULL);
}


void *threadClassCli(void *dados)
{
	balcUt *balcUtData = (balcUt *)dados;

	int res, prioLida, estadoFilho, x, res_checkFilas = -2;
	int fd_cli_fifo, fd_balc_fifo, res_size;
	int balClass[2], classBalc[2];
	char class_info[256], especLida[30], fimClass[5] = "#fim\n";
	char nome_fifo_cli[30];


	utente_ptr auxUtente = NULL;
	user cli_dados;


	//tratar classificador
	pipe(balClass);
	pipe(classBalc);

	res = fork();
	if (res == 0)
	{ 

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
			exit(3);
		}
		if(!balcUtData->flagContinua)
			break;

		pthread_mutex_lock(balcUtData->trinco);

		//se o cliente ainda não está registado, registar e falar com o classificador
		if (cli_dados.flagRegistado == 0)	
		{
			if ((*balcUtData->numUt) < balcUtData->maxUt)
			{				
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
				res_checkFilas = -2;

				if(strcmp(cli_dados.espec_atr, "oftalmologia") == 0){
					check_filaOftal(cli_dados, balcUtData->numUt, &res_checkFilas);
				}
				else if(strcmp(cli_dados.espec_atr, "neurologia") == 0){
					check_filaNeu(cli_dados, balcUtData->numUt, &res_checkFilas);
				}
				else if(strcmp(cli_dados.espec_atr, "estomatologia") == 0){
					check_filaEsto(cli_dados, balcUtData->numUt, &res_checkFilas);
				}
				else if(strcmp(cli_dados.espec_atr, "ortopedia") == 0){
					check_filaOrto(cli_dados, balcUtData->numUt, &res_checkFilas);
				}
				else if(strcmp(cli_dados.espec_atr, "geral") == 0){
					check_filaGe(cli_dados, balcUtData->numUt, &res_checkFilas);
				}

				//não deu para adicionar na fila
				if(res_checkFilas == -1){
					strcpy(cli_dados.mensagem, "BALC_FILA_NO_OK");
					printf("\nO cliente %s nao conseguiu ser registado.\n", cli_dados.nome);
				}
				//se deu para por na fila, por também no array geral
				else if(res_checkFilas >= 0){
					strcpy(cli_dados.mensagem, "BALC_FILA_OK");
					cli_dados.numUtFrente = res_checkFilas;

					auxUtente = add_ut(balcUtData->numUt, &cli_dados);
					array_ut = auxUtente;
					
					printf("\n[AVISO] O cliente %s foi registado na especialidade %s.\n\n", cli_dados.nome, cli_dados.espec_atr);
				}			
				
				//responder ao cliente
				write(fd_cli_fifo, &cli_dados, sizeof(user));
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
			if(strcmp(cli_dados.mensagem, "ADEUS") == 0){
				int auxPidUt = cli_dados.pidUt;
				array_ut = remover_ut(balcUtData->numUt, auxPidUt);				
			}
			else if(strcmp(cli_dados.mensagem, "SAIR") == 0){
				int aux = cli_dados.pidUt;
				array_ut = remover_ut(balcUtData->numUt, aux);
				arranja_filas(cli_dados.pidUt, cli_dados.espec_atr);
			}
		}
		
		pthread_mutex_unlock(balcUtData->trinco);

	}

	write(balClass[1], fimClass, strlen(fimClass)); //avisar classificador para terminar

	wait(&estadoFilho);

	pthread_exit(NULL);
}



void *threadMedico(void *dados)		
{
	int fd_balc_fifo2, fd_med_fifo;
	int res_size;
	char nome_fifo_med[30];

	balcMed *balcMedData = (balcMed *) dados;

	med_ptr auxMed = NULL;
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
			exit(3);
		}
		

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
					printf("[ERRO] Abrir o fifo do balc para o medico\n");
					exit(2);
				}

				//guardar o novo medico e responder
				auxMed = add_med(balcMedData->numMed, &med_dados);
				array_med = auxMed;

				printf("\n\n[AVISO] Medico %s foi registado na especialidade %s.\n\n", med_dados.nome, med_dados.especialidade);
				strcpy(med_dados.mensagem, "BALC_OK");
				write(fd_med_fifo, &med_dados, sizeof(medico));

			}
			else{
				//avisar medico que está no max de medicos
				strcpy(med_dados.mensagem, "MAXMED");

				sprintf(nome_fifo_med, MEDICO_FIFO, med_dados.med_id);
				fd_med_fifo = open(nome_fifo_med, O_WRONLY);
				if (fd_med_fifo == -1)
				{
					printf("[ERRO] Abrir o fifo do balc para o medico\n");
					exit(2);
				}
				write(fd_med_fifo, &med_dados, sizeof(medico));
			}
			
		}
		else{
			//aqui deve ser para ler os comandos do medico
			if(strcmp(med_dados.mensagem, "ADEUS") == 0){
				int aux = med_dados.med_id;

				for(int i = 0; i < *balcMedData->numMed; i++){
					if(array_med[i].medico_esp.med_id == aux){
						printf("\n[AVISO] A consulta entre o utente %s e o especialista %s terminou!\n\n", array_med[i].medico_esp.cliente_atual.nome, array_med[i].medico_esp.nome);
						array_med[i].medico_esp.flagEmConsulta = 0;
						break;
					}
				}


			}
			else if(strcmp(med_dados.mensagem, "SAIR") == 0){
				int aux = med_dados.med_id;
				array_med = remover_med(balcMedData->numMed, aux);
				printf("\n[AVISO] O medico %s saiu!\n\n", med_dados.nome);
			}
		}

		pthread_mutex_unlock(balcMedData->trinco);
	}

	pthread_exit(NULL);

}


//procurar medicos
void *threadProcMed(void *dados)		
{
	int fd_med_fifo;
	char nome_fifo_med[30];
	
	procuraMed *procuraMedData = (procuraMed *) dados;
	medico med_dados;


	while (procuraMedData->flagContinua)
	{
		sleep(5);
		for(int i = 0; i < *procuraMedData->numMed; i++ ){

			pthread_mutex_lock(procuraMedData->trinco);

			if (array_med[i].medico_esp.flagEmConsulta == 0)	
			{
				if (strcmp(array_med[i].medico_esp.especialidade, "oftalmologia") == 0 && Filas.oftalmologia[0].flagNaFila != -1)
				{
					int auxPid = 0;

					med_dados.cliente_atual = Filas.oftalmologia[0];

					sprintf(nome_fifo_med, MEDICO_FIFO, array_med[i].medico_esp.med_id);
					fd_med_fifo = open(nome_fifo_med, O_WRONLY);
					if (fd_med_fifo == -1)
					{
						printf("[ERRO] Abrir o fifo do balc para o cliente\n");
						exit(2);
					}
					strcpy(med_dados.mensagem, "RECEBE_DADOS_CONS");
					write(fd_med_fifo, &med_dados, sizeof(medico));

					array_med[i].medico_esp.flagEmConsulta = 1;
					array_med[i].medico_esp.cliente_atual = Filas.oftalmologia[0];
					auxPid = Filas.oftalmologia[0].pidUt;
					
					//encontrar o pid do cliente da fila mas no array geral para meter a flag a 1
					for(int j = 0; j < *procuraMedData->numUt; j++){

						if (array_ut[j].cliente.pidUt == auxPid)
						{
							array_ut[j].cliente.flagEmConsulta = 1;
							array_ut[j].cliente.flagNaFila = -1;
							strcpy(array_ut[j].cliente.medico_atr, array_med[i].medico_esp.nome);
							break;
						}
					}
					arranja_filas(auxPid, "oftalmologia");
				
				}
				else if(strcmp(array_med[i].medico_esp.especialidade, "neurologia") == 0 && Filas.neurologia[0].flagNaFila != -1){
					int auxPid = 0;

					med_dados.cliente_atual = Filas.neurologia[0];

					sprintf(nome_fifo_med, MEDICO_FIFO, array_med[i].medico_esp.med_id);
					fd_med_fifo = open(nome_fifo_med, O_WRONLY);
					if (fd_med_fifo == -1)
					{
						printf("[ERRO] Abrir o fifo do balc para o cliente\n");
						exit(2);
					}
					strcpy(med_dados.mensagem, "RECEBE_DADOS_CONS");
					write(fd_med_fifo, &med_dados, sizeof(medico));

					array_med[i].medico_esp.flagEmConsulta = 1;
					array_med[i].medico_esp.cliente_atual = Filas.neurologia[0];
					auxPid = Filas.neurologia[0].pidUt;
					
					for(int j = 0; j < *procuraMedData->numUt; j++){

						if (array_ut[j].cliente.pidUt == auxPid)
						{
							array_ut[j].cliente.flagEmConsulta = 1;
							array_ut[j].cliente.flagNaFila = -1;
							strcpy(array_ut[j].cliente.medico_atr, array_med[i].medico_esp.nome);
							break;
						}
					}
					arranja_filas(auxPid, "neurologia");
				}
				else if(strcmp(array_med[i].medico_esp.especialidade, "estomatologia") == 0 && Filas.estomatologia[0].flagNaFila != -1){
					
					int auxPid = 0;

					med_dados.cliente_atual = Filas.estomatologia[0];

					sprintf(nome_fifo_med, MEDICO_FIFO, array_med[i].medico_esp.med_id);
					fd_med_fifo = open(nome_fifo_med, O_WRONLY);
					if (fd_med_fifo == -1)
					{
						printf("[ERRO] Abrir o fifo do balc para o cliente\n");
						exit(2);
					}
					strcpy(med_dados.mensagem, "RECEBE_DADOS_CONS");
					write(fd_med_fifo, &med_dados, sizeof(medico));

					array_med[i].medico_esp.flagEmConsulta = 1;
					array_med[i].medico_esp.cliente_atual = Filas.estomatologia[0];
					auxPid = Filas.estomatologia[0].pidUt;
					
					for(int j = 0; j < *procuraMedData->numUt; j++){

						if (array_ut[j].cliente.pidUt == auxPid)
						{
							array_ut[j].cliente.flagEmConsulta = 1;
							array_ut[j].cliente.flagNaFila = -1;
							strcpy(array_ut[j].cliente.medico_atr, array_med[i].medico_esp.nome);
							break;
						}
					}
					arranja_filas(auxPid, "estomatologia");
				
				}
				else if(strcmp(array_med[i].medico_esp.especialidade, "ortopedia") == 0 && Filas.ortopedia[0].flagNaFila != -1){
					
					int auxPid = 0;

					med_dados.cliente_atual = Filas.ortopedia[0];

					sprintf(nome_fifo_med, MEDICO_FIFO, array_med[i].medico_esp.med_id);
					fd_med_fifo = open(nome_fifo_med, O_WRONLY);
					if (fd_med_fifo == -1)
					{
						printf("[ERRO] Abrir o fifo do balc para o cliente\n");
						exit(2);
					}
					strcpy(med_dados.mensagem, "RECEBE_DADOS_CONS");
					write(fd_med_fifo, &med_dados, sizeof(medico));

					array_med[i].medico_esp.flagEmConsulta = 1;
					array_med[i].medico_esp.cliente_atual = Filas.ortopedia[0];
					auxPid = Filas.ortopedia[0].pidUt;
					
					for(int j = 0; j < *procuraMedData->numUt; j++){

						if (array_ut[j].cliente.pidUt == auxPid)
						{
							array_ut[j].cliente.flagEmConsulta = 1;
							array_ut[j].cliente.flagNaFila = -1;
							strcpy(array_ut[j].cliente.medico_atr, array_med[i].medico_esp.nome);
							break;
						}
					}
					arranja_filas(auxPid, "ortopedia");
				
				}
				else if(strcmp(array_med[i].medico_esp.especialidade, "geral") == 0 && Filas.geral[0].flagNaFila != -1){
					
					int auxPid = 0;

					med_dados.cliente_atual = Filas.geral[0];

					sprintf(nome_fifo_med, MEDICO_FIFO, array_med[i].medico_esp.med_id);
					fd_med_fifo = open(nome_fifo_med, O_WRONLY);
					if (fd_med_fifo == -1)
					{
						printf("[ERRO] Abrir o fifo do balc para o cliente\n");
						exit(2);
					}
					strcpy(med_dados.mensagem, "RECEBE_DADOS_CONS");
					write(fd_med_fifo, &med_dados, sizeof(medico));

					array_med[i].medico_esp.flagEmConsulta = 1;
					array_med[i].medico_esp.cliente_atual = Filas.geral[0];
					auxPid = Filas.geral[0].pidUt;
					
					//encontrar o pid do cliente da fila mas no array geral para meter a flag a 1
					for(int j = 0; j < *procuraMedData->numUt; j++){

						if (array_ut[j].cliente.pidUt == auxPid)
						{
							array_ut[j].cliente.flagEmConsulta = 1;
							array_ut[j].cliente.flagNaFila = -1;
							strcpy(array_ut[j].cliente.medico_atr, array_med[i].medico_esp.nome);
							break;
						}
					}
					arranja_filas(auxPid, "geral");
				}
			}
			pthread_mutex_unlock(procuraMedData->trinco);	
		}
	}
	pthread_exit(NULL);
}


int main(int argc, char *argv[], char *envp[])
{
	setbuf(stdout, NULL);

	int res_size, flag_fimClass = 0;
	int fd_balc_fifo, fd_balc_fifo2;
	char comando[30], sendExit = 'E';
	char *comAux, *nomeRemo, *auxNomeRemo;
	char *maxClientes, *maxMedicos;
	int max_clientes, max_medicos;
	int numUt = 0, numMed = 0, indice, primeiraVez = 0;;


	balcUt balcUtData;

	balcMed balcMedData;

	procuraMed procuraMedData;

	freqN freqData;

	pthread_t thread_classCli;
	pthread_t thread_med;
	pthread_t thread_procMed;
	pthread_t thread_freqN;
	pthread_mutex_t trinco;


	//----Tratar Variáveis Ambiente------

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

	//criar fifo balcao-cliente e balcao-medico
	if (access(BALCAO_FIFO, F_OK) != 0)
	{
		mkfifo(BALCAO_FIFO, 0600);
	}
	if (access(BALCAO_FIFO2, F_OK) != 0)
	{
		mkfifo(BALCAO_FIFO2, 0600);
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

	
	pthread_mutex_init(&trinco, NULL);


	//iniciar estrutura das filas	
	for(int i = 0; i < 5; i++){
		Filas.oftalmologia[i].flagNaFila = -1;
		Filas.ortopedia[i].flagNaFila = -1;
		Filas.neurologia[i].flagNaFila = -1;
		Filas.estomatologia[i].flagNaFila = -1;
		Filas.geral[i].flagNaFila = -1;
	}


	balcUtData.flagContinua = 1;
	balcUtData.trinco = &trinco;
	balcUtData.maxUt = max_clientes;
	balcUtData.numUt = &numUt;


	balcMedData.flagContinua = 1;
	balcMedData.trinco = &trinco;
	balcMedData.maxMed = max_medicos;
	balcMedData.numMed = &numMed;


	procuraMedData.trinco = &trinco;
	procuraMedData.flagContinua = 1;
	procuraMedData.numMed = &numMed;
	procuraMedData.numUt = &numUt;

	freqData.segundos = 30;
	freqData.flagContinua = 1;
	freqData.trinco = &trinco;

	//thread para ouvir cliente
	pthread_create(&thread_classCli, NULL, threadClassCli, (void *) &balcUtData);
	
	//thread para ouvir medico
	pthread_create(&thread_med, NULL, threadMedico, (void*) &balcMedData);

	//thread para procurar medico livre
	pthread_create(&thread_procMed, NULL, threadProcMed, (void*) &procuraMedData);

	//chamar thread do freq
	pthread_create(&thread_freqN, NULL, threadFreqN, (void*) &freqData);


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

			//parar thread procura medico
			pthread_mutex_lock(procuraMedData.trinco);
			procuraMedData.flagContinua = 0;
			pthread_mutex_unlock(procuraMedData.trinco);

			//parar thread do freq
			pthread_mutex_lock(freqData.trinco);
			freqData.flagContinua = 0;
			pthread_mutex_unlock(freqData.trinco);

			//desbloquear read
			write(fd_balc_fifo, &(sendExit), sizeof(char));
			write(fd_balc_fifo2, &(sendExit), sizeof(char));

			encerra_todos(numMed, numUt);
			
			break;
		}
		else if(strcmp(comando, "utentes\n") == 0){
			printf("\n[A Listar utentes em fila de espera e a serem atendidos]\n");
			listaUtentes(array_ut, numUt);
		}
		else if(strcmp(comando, "especialistas\n") == 0){
			printf("\n[A listar todos os especialistas]\n");
			lista_med(array_med, numMed);
		}
		else{
			comAux = strtok_r(comando, " ", &auxNomeRemo);
			nomeRemo = auxNomeRemo;
			nomeRemo[strcspn(nomeRemo, "\n")] = 0;

			if(strcmp(comAux, "delut") == 0){
				delutX(nomeRemo, &numUt);
			}
			else if (strcmp(comAux, "delesp") == 0)
			{
				delespX(nomeRemo, &numMed);	
			}
			else if(strcmp(comAux, "freq") == 0){
				int segundos = atoi(nomeRemo);

				pthread_mutex_lock(freqData.trinco);
				freqData.segundos = segundos;
				pthread_mutex_unlock(freqData.trinco);
			}
			else{
				printf("O comando nao existe!\n");
			}
			
		}
	}

	printf("[AVISO] Classificador a terminar...\n");
	printf("[AVISO] Balcao a encerrar...\n");

	pthread_join(thread_classCli, NULL);	
	pthread_join(thread_med, NULL);
	pthread_join(thread_procMed, NULL);
	pthread_join(thread_freqN, NULL);

	pthread_mutex_destroy(&trinco);

	unlink(BALCAO_FIFO);

	exit(0);
}
