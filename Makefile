.PHONY: run

run: main
	./main.o $(args)

main: main.c
	gcc -W -Wall -pedantic -std=c11 main.c -o main.o -g