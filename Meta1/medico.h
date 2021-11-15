#ifndef MEDICO_H
#define MEDICO_H

#include "cliente.h"

#define  BALCAO_FIFO "balcFifo"
#define  UTENTE_FIFO "utenteFifo_%d"
#define  MED_FIFO "medFifo_%d"

#define TAM 40


typedef struct Medico medico, *medico_ptr;
struct Medico{

	char nome[TAM];
	char especialidade[TAM];
	int med_id;
	user_ptr cliente_atual;

};

#endif 
