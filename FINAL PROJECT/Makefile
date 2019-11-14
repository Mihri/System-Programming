all: 
	gcc -o BibakBOXServer restart.c server.c -lpthread  -lm
	gcc -o BibakBOXClient restart.c client.c -lpthread  -lm

restart.o: restart.c 
	gcc -c restart.c 

clean:
	rm *.o BibakBOXServer BibakBOXClient

