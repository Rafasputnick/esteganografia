.PHONY: run open format clean clean_f 

run: fusermount.out
	sudo mkdir /mnt/device1
	./fusermount.out -f /mnt/device1 original-zebras.bmp init
	sudo rm -d /mnt/device1

open:
	cd /mnt/device1

fusermount.out: fusemount.c fusemount.h bmp.c bmp.h lsb.c lsb.h
	gcc -W -Wall -pedantic -g fusemount.c fusemount.h bmp.c bmp.h lsb.c lsb.h -o fusermount.out `pkg-config fuse --cflags --libs`
	
format:
	clang-format -i -style=file *.c *.h

clean:
	fusermount -u ./dir1

clean_f:
	sudo umount -l /mnt/device1
