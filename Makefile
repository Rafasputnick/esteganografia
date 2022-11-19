.PHONY: run open format umount umount_f test

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

umount:
	fusermount -u ./dir1

umount_f:
	sudo umount -l /mnt/device1

test:
	mkdir ./dir1/pasta1
	mkdir ./dir1/pasta2
	echo opa > ./dir1/arq1
	echo opa2 > ./dir1/arq2
	ls ./dir1