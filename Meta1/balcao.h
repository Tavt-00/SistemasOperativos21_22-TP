#ifndef BALCAO_H
#define BALCAO_H
 

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
#include "cliente.h"
#include "medico.h"
#include <limits.h>


#define TAM 40


#define  BALCAO_FIFO "balcFifo"
#define  UTENTE_FIFO "utenteFifo_%d"
#define  MED_FIFO "medFifo_%d"


typedef struct Utente utente, *utente_ptr;
struct Utente{

	user cliente;
	//medico medico_esp;
	int flag_emConsulta;
};



typedef struct Med med, *med_ptr;
struct Med{

	medico medico_esp;
};


typedef struct Balcao balcao, *balc_ptr;
struct Balcao{

	utente_ptr array_utentes;
	med_ptr array_medicos;
	int *numMed_atual;
	int *numUten_atual;
	int max_utentes;
	int max_medicos;
};



#endif