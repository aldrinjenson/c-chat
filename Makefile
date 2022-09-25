all:
	gcc -o client.out client.c implementations/*
	gcc -o server.out server.c implementations/*
	./server.out 5000