# Makefile for Quash

all: quash

quash: 
	gcc -Wall src/execute.c src/jobs.c src/builtin.c src/pipes.c -o quash

clean: 
	rm -f quash

valgrind: quash
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./quash
