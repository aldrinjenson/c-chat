all:
	gcc -o client.out client.c implementations/*
	gcc -o server.out server.c implementations/*
server:
	./server.out 5000
client:
	./client.out 5000