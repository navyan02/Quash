# all: quash

# quash: 
# 	gcc -Wall src/main.c src/parser.c src/builtin.c src/execute.c -o quash

# clean: 
# 	rm -f quash

# Makefile for Quash


all: quash

quash: quash.o
	gcc -Wall -g -o quash quash.o

quash.o: quash.c
	gcc -Wall -g -c quash.c

clean:
	rm -f *.o quash
