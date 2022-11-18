.PHONY: run debugGeral clean clean_f

debugGeral:
	gcc -W -Wall -pedantic -g testeFinal/geral.c -o testeFinal/geral `pkg-config fuse --cflags --libs`
	

clean:
	fusermount -u ./dir1

clean_f:
	sudo umount -l testeFinal/dir1

run: main.out
	./main.out original-zebras.bmp -s

main.out: main.c
	gcc -W -Wall -pedantic -std=c11 main.c -o main.out -g