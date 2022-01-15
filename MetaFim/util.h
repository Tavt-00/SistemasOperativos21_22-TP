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
	int flagNaFila;
	int flagRegistado;
	int flagEmConsulta;
	int prioridade_atr;
	int numUtFrente;
	int pid_medAtr;
	int flagInicioCons;
	
	char nome[50];
	char mensagem[50];
	char chat[50];
	char espec_atr[50];
	char medico_atr[50];
};


typedef struct Utente utente, *utente_ptr;
struct Utente{

	user cliente;
};


//MEDICO
typedef struct Medico medico, *medico_ptr;
struct Medico{

	char nome[50];
	char especialidade[50];
	char mensagem[50];
	char chat[50];
	int med_id;
	int flagRegistado;
	int flagEmConsulta;
	user cliente_atual;

};


typedef struct Med med, *med_ptr;
struct Med{

	medico medico_esp;
};


typedef struct  utenteMedico utMed;
struct utenteMedico{
	int pid;	
	char mensagem[50];
};


typedef struct filasEspera filas, *filas_ptr;
struct filasEspera{

	user oftalmologia[5];
	user neurologia[5];
	user estomatologia[5];
	user ortopedia[5];
	user geral [5];

};


typedef struct utentesAtendidos utAtend, *utAtend_ptr;
struct utentesAtendidos{

	utente_ptr array_utAtendidos; //aray dos utentes a serem atendidos

};


//COMUNICAÇÃO
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


	pthread_mutex_t *trinco;
};


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
};


//para a thread que procura no array dos medicos algum livre 
typedef struct procuraMedico procuraMed, *procuraMed_ptr;
struct procuraMedico{
	
	int flagContinua;
	int *numMed;
	int *numUt;
	
	pthread_mutex_t *trinco;
};



#endif 