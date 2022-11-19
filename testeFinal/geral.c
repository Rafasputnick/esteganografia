// Desenvolvido por Rafael Nascimento Lourencobmp_header
// Referencias
// https://docs.fileformat.com/image/bmp/
// https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure
// https://en.wikipedia.org/wiki/BMP_file_format
// https://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/


#include <errno.h>
#include <fuse.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Manipulando o bmp



