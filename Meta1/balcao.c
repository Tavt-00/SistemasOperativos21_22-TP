#include "balcao.h"


int main(int argc, char *argv[], char *envp[])
{
	
	int res, estadoFilho;
	char comando[30], fimClass[5] = "#fim\n" ;
	int balClass[2];
	int classBalc[2];
	char *maxClientes, *maxMedicos;
	int max_clientes, max_medicos;
	char class_info[30];
	char especiliade[][30] = {"oftalmologia" ,"neurologia", "estomatologia", "ortopedia", "geral"};


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


	printf("Coloque os seus sintomas: \n");
	res = fork();
	if(res == 0){	//filho
		printf("[FILHO] - Sou o %d. O meu pai e o %d.\n", getpid(), getppid());
		
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

	do{
		printf("[PAI] Criei um filho %d...\n", res);
		printf("Comandos: ");
		fflush(stdout);
		fgets(comando, sizeof(comando), stdin);
		printf("\n |%s|",comando);

		if (strcmp(comando,"encerra\n") == 0){
			write(balClass[1], fimClass, strlen(fimClass));
			printf("222222\n");
			break;
		}
		else {
			write(balClass[1], comando, strlen(comando));
			read(classBalc[0], class_info, sizeof(class_info));
			printf("Resultado do classificador: %s\n", class_info);
			memset(class_info, '\0', 30);
		}
		printf("1111\n");
		
		
	}while(1); 

	printf("OLALAL\n");
    wait(&estadoFilho);
    close(balClass[1]);
	close(classBalc[0]);
	printf("[PAI] O meu filho terminou com: %d\n", WEXITSTATUS(estadoFilho));		


	exit(0);
}