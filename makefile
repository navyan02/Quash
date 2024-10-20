# Makefile for Quash

all: quash

quash: 
	gcc -Wall src/main.c src/parser.c src/builtin.c src/execute.c -o quash

clean: 
	rm -f quash

valgrind: quash
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./quash