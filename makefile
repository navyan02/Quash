# Makefile for Quash

all: quash

quash: 
	gcc -Wall src/main.c src/parser.c src/builtin.c src/execute.c -o quash

clean: 
	rm -f quash

