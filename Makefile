.PHONY: run format clean clean_f

run: fusermount.out fusermount.c fusermount.h bmp.c bmp.h lsb.c lsb.h
	sudo mkdir /mnt/device1
	./fusermount.out -f /mnt/device1 original-zebras.bmp init
	sudo rm -d /mnt/device1

fusermount.out:
	gcc -W -Wall -pedantic -g fusermount.c fusermount.h bmp.c bmp.h lsb.c lsb.h -o fusermount.out `pkg-config fuse --cflags --libs`
	
format:
	clang-format -i -style=file *.c *.h

clean:
	fusermount -u /mnt/device1

clean_f:
	sudo umount -l /mnt/device1
