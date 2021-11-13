#include "balcao.h"


int main(int argc, char *argv[])
{
	
	int res, estadoFilho;
	int balClass[2];
	int classBalc[2];
	char especiliade[][20] = {"oftalmologia" ,"neurologia", "estomatologia", "ortopedia", "geral"};


	pipe(balClass);
	pipe(classBalc);

	printf("Coloque os seus sintomas: \n");

		//entra aqui com o input dos sintomas
	
	res = fork();
	if(res == 0){	//filho
		printf("[FILHO] - Sou o %d. O meu pai e o %d.\n", getpid(), getppid());
		
		dup(classBalc[1]);
   	 	close(classBalc[1]);
    	close(classBalc[0]);

    	dup(balClass[0]);
    	close(balClass[0]);
    	close(balClass[1]);
		
		execl("classificador", "classificador", NULL);
				//se vier para aqui o exec correu mal
		printf("[FILHO] Exec correu mal!");
		exit(123);	
	}
	else{
		printf("[PAI] Criei um filho %d...\n", res);
		
		dup(classBalc[0]);
   		close(classBalc[0]);
    	close(classBalc[1]);

  	 	dup(balClass[1]);
    	close(balClass[1]);
    	close(balClass[0]);

    	wait(&estadoFilho);
		
		printf("[PAI] O meu filho terminou com: %d\n", WEXITSTATUS(estadoFilho));	
	}


	

	exit(0);
}