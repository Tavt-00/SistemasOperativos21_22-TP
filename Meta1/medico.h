#ifndef MEDICO_H
#define MEDICO_H

#include "cliente.h"

#define TAM 40


typedef struct Medico medico, *medico_ptr;
struct Medico{

	char nome[TAM];
	char especialidade[TAM];
	int med_id;
	user_ptr cliente_atual;

};

#endif 
