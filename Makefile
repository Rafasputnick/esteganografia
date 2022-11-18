.PHONY: run debugGeral clean

debugGeral:
	gcc -g testeFinal/geral.c -o testeFinal/geral `pkg-config fuse --cflags --libs`
	

clean:
	sudo umount -l testeFinal/dir1

run: main.out
	./main.out original-zebras.bmp -s

main.out: main.c
	gcc -W -Wall -pedantic -std=c11 main.c -o main.out -g