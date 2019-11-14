all: server client 

server: server.o 
	gcc -o server server.o -lm

client: client.o
	gcc -o client client.o -lm

server.o: server.c
	gcc -c server.c

client.o: client.c
	gcc -c client.c



clean:
	rm *.o *.fifo server client; rm -rf log

run1: 
	./server  

run2: 
	./client