all: cliente balcao medico 

cliente: cliente.c util.h
	gcc -o cliente cliente.c 

balcao: balcao.c util.h
	gcc -lpthread -o balcao balcao.c 

medico: medico.c util.h
	gcc -o medico medico.c

clean:
	rm cliente balcao medico 
