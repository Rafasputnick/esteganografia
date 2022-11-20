.PHONY: run open format umount umount_f test print

run: fusermount.out
	mkdir -p $(dir) 
	./fusermount.out -f $(dir) -d -s $(img) init

fusermount.out: fusermount.c fusermount.h bmp.c bmp.h lsb.c lsb.h
	gcc -W -Wall -pedantic -g fusermount.c fusermount.h bmp.c bmp.h lsb.c lsb.h -o fusermount.out `pkg-config fuse --cflags --libs`

open:
	mkdir -p $(dir) 
	./fusermount.out -f $(dir) -d -s $(img)
		
format:
	clang-format -i -style=file *.c *.h

umount:
	fusermount -u $(dir)
	rmdir $(dir)

umount_f:
	sudo umount -l $(dir)
	rmdir $(dir)

test:
	mkdir $(dir)/pasta1
	mkdir $(dir)/pasta2
	echo 123456789 > $(dir)/arq1.txt
	echo 123456789 > $(dir)/arq2.txt
	echo 123456789 > $(dir)/arq3.txt
	echo 123456789 > $(dir)/arq4.txt
	echo 123456789 > $(dir)/arq5.txt
	echo 123456789 > $(dir)/arq6.txt
	echo 123456789 > $(dir)/arq7.txt
	echo 123456789 > $(dir)/arq8.txt
	echo 123456789 > $(dir)/arq9.txt
	echo 123456789 > $(dir)/arq10.txt
	ls $(dir)

print:
	cat $(dir)/arq1.txt
	cat $(dir)/arq2.txt
	cat $(dir)/arq3.txt
	cat $(dir)/arq4.txt
	cat $(dir)/arq5.txt
	cat $(dir)/arq6.txt
	cat $(dir)/arq7.txt
	cat $(dir)/arq8.txt
	cat $(dir)/arq9.txt
	cat $(dir)/arq10.txt
	ls $(dir)/pasta1
	ls $(dir)/pasta2
