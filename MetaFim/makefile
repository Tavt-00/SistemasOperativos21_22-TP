all: cliente balcao medico 

cliente: cliente.c cliente.h
	gcc -o cliente cliente.c 

balcao: balcao.c balcao.h cliente.h medico.h
	gcc -o balcao balcao.c 

medico: medico.c medico.h cliente.h
	gcc -o medico medico.c

clean:
	rm cliente balcao medico 