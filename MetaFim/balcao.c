#include "util.h"

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



int main(int argc, char *argv[], char *envp[])
{
	
	int res, estadoFilho, flag_fimClass = 0;
	char comando[30], fimClass[5] = "#fim\n" ;
	int balClass[2],classBalc[2];
	char *maxClientes, *maxMedicos;
	int max_clientes, max_medicos;
	char class_info[30];
	//char especialiades[][30] = {"oftalmologia" ,"neurologia", "estomatologia", "ortopedia", "geral"};

	char especLida[30];
	int prioLida;
	user newUser;  
	utente_ptr array_ut = NULL; //ponteiro para array de players

	pipe(balClass);
	pipe(classBalc);

	//----Tratar Variáveis Ambiente-------\\

	maxClientes = getenv("MAXCLIENTES");
	if (maxClientes == NULL)
	{
		printf("[Erro] MAXCLIENTES nao existe!\n");
		exit(1);		
	}
	else{
		max_clientes = atoi(maxClientes);
	}

	maxMedicos = getenv("MAXMEDICOS");
	if (maxMedicos == NULL)
	{
		printf("[Erro] MAXMEDICOS nao existe!\n");
		exit(1);		
	}
	else{
		max_medicos = atoi(maxMedicos);
	}


	printf("Coloque os seus sintomas ou digite comandos \n");
	res = fork();
	if(res == 0){	//filho
		
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

	do{
		printf("\nComandos: ");
		fflush(stdout);
		fgets(comando, sizeof(comando), stdin);


		if (strcmp(comando,"encerra\n") == 0 && !flag_fimClass){	//encerrar servidor com class a correr 
			write(balClass[1], fimClass, strlen(fimClass));	//avisar class para terminar
			printf("[AVISO] Classificador a terminar...\n");
			break;
		}
		else if(strcmp(comando,"encerra\n") == 0 && flag_fimClass){	//encerrar servidor com class terminado
			break;
		}
		else if(strcmp(comando, "#fim\n") == 0 && !flag_fimClass) {	//se for para parar o classificador pela 1ª vez
			flag_fimClass = 1;
			printf("[AVISO] Classificador a terminar...\n");
			write(balClass[1], comando, strlen(comando));	//mandar classificador parar
		}
		else if(strcmp(comando, "#fim\n") == 0 && flag_fimClass) {	//se voltar a pedir para parar classificador mas este já parou
			printf("[AVISO] Classificador ja terminou anteriormente!\n");
		}
		else if(!flag_fimClass) {	//se for para mandar p/classificador que ainda se encontra a correr
			write(balClass[1], comando, strlen(comando));
			read(classBalc[0], class_info, sizeof(class_info));

				//guardar resposta do class no user
			sscanf(class_info, "%s %d", especLida, &prioLida);	
			newUser.prioridade_atr = prioLida;
			strcpy(newUser.espec_atr, especLida);

			printf("Especialidade atribuida: %s\nPrioridade atribuida: %d\n", especLida, prioLida);

			memset(class_info, '\0', 30);
		}
		else {	
			printf("Comando apenas lido, classificador esta encerrado\n");
		}
	}while(1); 

	printf("[AVISO] Balcao a encerrar...\n");

    wait(&estadoFilho);

	exit(0);
}
