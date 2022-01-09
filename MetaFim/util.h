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

#define  BALCAO_FIFO "balcFifo"
#define  UTENTE_FIFO "utenteFifo_%d"
#define  MED_FIFO "medFifo_%d"

#define TAM 40


//CLIENTE
typedef struct User user, *user_ptr;
struct User{

	int pidUt;
	int flagContinua;
	int flagSintoma;
	char nome[50];
	char mensagem[50];
	int prioridade_atr;
	char espec_atr[50];
};

//CLIENTE
typedef struct Usertemp usertemp, *user_ptrTemp;
struct Usertemp{

	char nome[TAM];
	char sintoma[TAM];
	char espec_atr[TAM];
	int prioridade_atr;
	int numUtente_frente;
	int user_id;
	int pidMed_atr;
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
	int med_id;
	user_ptr cliente_atual;

};


typedef struct Med med, *med_ptr;
struct Med{

	medico medico_esp;
	int flag_emConsulta;
};


//BALCAO
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
typedef struct balcaoMedico balcMed;
struct balcaoMedico{
	int pidMed;
	char especialidade[50];
	char nomeMed[50];
	char mensagem[50];
};


typedef struct balcaoUtente balcUt;
struct balcaoUtente{
	int pidUt;
	int flagContinua;
	int flagSintoma;
	char nomeUt[50];
	char mensagem[50];
	int prioridade_atr;
	char espec_atr[50];

	utente_ptr array_utentes;

	pthread_mutex_t *trinco;
};


typedef struct  utenteMedico utMed;
struct utenteMedico{
	int pid;	
	char mensagem[50];
};


#endif 