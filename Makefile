.PHONY: run debugGeral

debugGeral:
	gcc -g testeFinal/geral.c -o testeFinal/geral `pkg-config fuse --cflags --libs`
	testeFinal/geral -f testeFinal/dir1/ testeFinal/original-zebras_copy.bmp -s


clean:
	sudo umount -l testeFinal/dir1

run: main.out
	./main.out original-zebras.bmp -s

main.out: main.c
	gcc -W -Wall -pedantic -std=c11 main.c -o main.out -g