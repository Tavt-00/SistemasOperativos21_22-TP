#ifndef CLIENTE_H
#define CLIENTE_H

#define  BALCAO_FIFO "balcFifo"
#define  UTENTE_FIFO "utenteFifo_%d"
#define  MED_FIFO "medFifo_%d"

#define TAM 40

typedef struct User user, *user_ptr;
struct User{

	char nome[TAM];
	char sintoma[TAM];
	char espec_atr[TAM];
	int prioridade_atr;
	int numUtente_frente;
	int user_id;
	int pidMed_atr;
};


#endif 