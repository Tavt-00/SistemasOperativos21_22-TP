#ifndef UTIL_H
#define UTIL_H
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <pthread.h>

#define  BALCAO_FIFO "balcFifoUt"		//mudar aqui tudo depois para BALCAO_FIFO1
#define  BALCAO_FIFO2 "balcFifoMed"
#define  UTENTE_FIFO "utenteFifo_%d"
#define  MEDICO_FIFO "medicoFifo_%d"

#define TAM 40


//CLIENTE
typedef struct User user, *user_ptr;
struct User{

	int pidUt;
	int flagSintoma;
	int flagRegistado;
	char nome[50];
	char mensagem[50];
	int prioridade_atr;
	char espec_atr[50];
};


typedef struct Utente utente, *utente_ptr;
struct Utente{

	user cliente;
	int flag_emConsulta;
};


//MEDICO
typedef struct Medico medico, *medico_ptr;
struct Medico{

	char nome[TAM];
	char especialidade[TAM];
	char mensagem[50];
	int med_id;
	int flagRegistado;
	user_ptr cliente_atual;

};


typedef struct Med med, *med_ptr;
struct Med{

	medico medico_esp;
	int flag_emConsulta;
};


//BALCAO -- TIRAR ISTO??????
typedef struct Balcao balcao, *balc_ptr;
struct Balcao{

	utente_ptr array_utentes;
	med_ptr array_medicos;
	int *numMed_atual;
	int *numUten_atual;
	int max_utentes;
	int max_medicos;
};





//COMUNICAÇÃO
typedef struct balcaoMedico balcMed, *balcMed_ptr;
struct balcaoMedico{
	char especialidade[50];
	char nomeMed[50];
	char mensagem[50];

	int flagContinua;
	int pidMed;
	int *numMed;
	int maxMed;

	pthread_mutex_t *trinco;

	med_ptr array_medicos;
};


typedef struct balcaoUtente balcUt, *balcUt_ptr;
struct balcaoUtente{
	int flagSintoma;
	char nomeUt[50];
	char mensagem[50];
	int prioridade_atr;
	char espec_atr[50];

	int flagContinua;
	int pidUt;
	int *numUt;
	int maxUt;

	utente_ptr array_utentes;

	pthread_mutex_t *trinco;
};


typedef struct  utenteMedico utMed;
struct utenteMedico{
	int pid;	
	char mensagem[50];
};


typedef struct filasEspera filas, *filas_ptr;
struct filasEspera{

	utente_ptr oftalmologia[4];
	utente_ptr neurologia[4];
	utente_ptr estomatologia[4];
	utente_ptr ortopedia[4];
	utente_ptr geral [4];

};


#endif 