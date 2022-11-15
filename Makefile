.PHONY: run

run: main.out
	./main.out original-zebras.bmp -s

main.out: main.c
	gcc -W -Wall -pedantic -std=c11 main.c -o main.out -g