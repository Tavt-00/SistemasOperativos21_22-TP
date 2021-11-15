#include "balcao.h"


int main(int argc, char *argv[], char *envp[])
{
	
	int res, estadoFilho;
	int balClass[2];
	int classBalc[2];
	char *maxClientes, *maxMedicos;
	int max_clientes, max_medicos;
	char especiliade[][20] = {"oftalmologia" ,"neurologia", "estomatologia", "ortopedia", "geral"};


	pipe(balClass);
	pipe(classBalc);

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

		//entra aqui com o input dos sintomas
	
	res = fork();
	if(res == 0){	//filho
		//printf("[FILHO] - Sou o %d. O meu pai e o %d.\n", getpid(), getppid());
		
		execl("classificador", "classificador", NULL);
				//se vier para aqui o exec correu mal
		printf("Exec correu mal!");
		exit(123);	
	}
	else{
		//printf("[PAI] Criei um filho %d...\n", res);
    	wait(&estadoFilho);
		
		//printf("[PAI] O meu filho terminou com: %d\n", WEXITSTATUS(estadoFilho));	
	}

	exit(0);
}