// Desenvolvido por Rafael Nascimento Lourenco
// Referencias
// https://docs.fileformat.com/image/bmp/
// https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure
// https://en.wikipedia.org/wiki/BMP_file_format
// https://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/
#define FUSE_USE_VERSION 30

#include <errno.h>
#include <fuse.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Manipulando o bmp

int getBit(uint8_t byte, uint8_t bit) {
  uint8_t bitsAtRigth = bit - 1;
  uint8_t shefittedByte = byte >> bitsAtRigth;
  return shefittedByte & 1;
}


typedef union {
  unsigned char bytes[2];
  int16_t valor;
} byteIntoTwoBytes;


char dir_list[256][256];
int16_t curr_dir_idx = -1;

char **files_list[256][256];
int16_t curr_file_idx = -1;

char **files_content[256][256];
int16_t curr_file_content_idx = -1;

char message[256] = "Isto eh uma mensagem";

typedef struct BmpHeader {
  char *fileType;
  uint32_t fSize;
  uint16_t *reserveds;
  uint32_t bitmapAddress;
} bmp_header;

typedef struct DibHeader {
  uint32_t headerSize;
  uint16_t bmWidth;
  uint16_t bmHeight;
  uint16_t colorPlanes;
  uint16_t bitPerPixel;
} dib_header;

void exitWithError(FILE *filePointer, char *msg) {
  printf("%s\n", msg);
  fclose(filePointer);
  exit(1);
}

void getBytesFromFile(FILE *filePointer, char *value, uint32_t howMuchBytes) {
  fread(value, 1, howMuchBytes, filePointer);
}

void getBmpHeader(FILE *filePointer, bmp_header *header) {
  getBytesFromFile(filePointer, header->fileType, 2);

  getBytesFromFile(filePointer, ((char *)&header->fSize), 4);

  getBytesFromFile(filePointer, ((char *)header->reserveds), 2);
  getBytesFromFile(filePointer, ((char *)header->reserveds + 1), 2);

  getBytesFromFile(filePointer, ((char *)&header->bitmapAddress), 4);
}

void getDibHeader(FILE *filePointer, dib_header *header) {

  getBytesFromFile(filePointer, ((char *)&header->headerSize), 4);

  getBytesFromFile(filePointer, ((char *)&header->bmWidth), 4);

  getBytesFromFile(filePointer, ((char *)&header->bmHeight), 4);

  getBytesFromFile(filePointer, ((char *)&header->colorPlanes), 2);

  getBytesFromFile(filePointer, ((char *)&header->bitPerPixel), 2);
}

void initialize() {
  dir_list = malloc(sizeof(*dir_list) * 256);
  files_list = malloc(sizeof(*files_list) * 256);
  files_content = malloc(sizeof(*files_content) * 256);

  for (int i = 0; i < 256; i++) {
    dir_list[i] = malloc(sizeof(*(dir_list[i])) * 256);
    files_list[i] = malloc(sizeof(*(files_list[i])) * 256);
    files_content[i] = malloc(sizeof(*(files_content[i])) * 256);
  }
}

void freeMatrixs() {
  for (int i = 255; i >= 0; i--) {
    free(dir_list[i]);
    free(files_list[i]);
    free(files_content[i]);
  }
  free(dir_list);
  free(files_list);
  free(files_content);
}

void putByteWithLsbMethod(int8_t byteForLsb, FILE *filePointer) {
  int8_t cur_byte;
  for (int i = 1; i < 9; i++) {
    fread(&cur_byte, 1, 1, filePointer);
    fseek(filePointer, -1, SEEK_CUR);
    if (getBit(byteForLsb, i)) {
      if (cur_byte % 2 == 0) {
        cur_byte++;
      }
    } else {
      if (cur_byte % 2 == 1) {
        cur_byte--;
      }
    }
    fputc(cur_byte, filePointer);
  }
}

int8_t getByteWithLsbMethod(FILE *filePointer) {
  int8_t res = 0;
  int8_t byteFromFile;

  for (int i = 0; i < 8; i++) {
    fread(&byteFromFile, 1, 1, filePointer);
    if (byteFromFile % 2 == 1) {
      res |= 1 << i;
    }
  }

  return res;
}

void createLsbMethodHeader(FILE *filePointer) {
  byteIntoTwoBytes aux;
  aux.valor = -1;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      putByteWithLsbMethod(aux.bytes[j], filePointer);
    }
  }
}

void readLsbMethodHeader(FILE *filePointer) {
  byteIntoTwoBytes aux;

  aux.bytes[0] = getByteWithLsbMethod(filePointer);
  aux.bytes[1] = getByteWithLsbMethod(filePointer);
  curr_dir_idx = aux.valor;

  aux.valor = 0;

  aux.bytes[0] = getByteWithLsbMethod(filePointer);
  aux.bytes[1] = getByteWithLsbMethod(filePointer);
  curr_file_idx = aux.valor;

  aux.valor = 0;

  aux.bytes[0] = getByteWithLsbMethod(filePointer);
  aux.bytes[1] = getByteWithLsbMethod(filePointer);
  curr_file_content_idx = aux.valor;
}

/////////////////////////////////////////////////////////////////////

// Trabalhando com o diretorio (libfuse)

void add_dir(const char *dir_name) {
  curr_dir_idx++;
  strcpy(dir_list[curr_dir_idx], dir_name);
}

int is_dir(const char *path) {
  path++; // Eliminating "/" in the path

  for (int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++)
    if (strcmp(path, dir_list[curr_idx]) == 0)
      return 1;

  return 0;
}

void add_file(const char *filename) {
  curr_file_idx++;
  strcpy(files_list[curr_file_idx], filename);

  curr_file_content_idx++;
  strcpy(files_content[curr_file_content_idx], "");
}

int is_file(const char *path) {
  path++; // Eliminating "/" in the path

  for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
    if (strcmp(path, files_list[curr_idx]) == 0)
      return 1;

  return 0;
}

int get_file_index(const char *path) {
  path++; // Eliminating "/" in the path

  for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
    if (strcmp(path, files_list[curr_idx]) == 0)
      return curr_idx;

  return -1;
}

void write_to_file(const char *path, const char *new_content) {
  int file_idx = get_file_index(path);

  if (file_idx == -1) // No such file
    return;

  strcpy(files_content[file_idx], new_content);
}

static int do_getattr(const char *path, struct stat *st) {
  st->st_uid = getuid(); // The owner of the file/directory is the user who
                         // mounted the filesystem
  st->st_gid = getgid(); // The group of the file/directory is the same as the
                         // group of the user who mounted the filesystem
  st->st_atime =
      time(NULL); // The last "a"ccess of the file/directory is right now
  st->st_mtime =
      time(NULL); // The last "m"odification of the file/directory is right now

  if (strcmp(path, "/") == 0 || is_dir(path) == 1) {
    st->st_mode = S_IFDIR | 0755;
    st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is
                      // here: http://unix.stackexchange.com/a/101536
  } else if (is_file(path) == 1) {
    st->st_mode = S_IFREG | 0644;
    st->st_nlink = 1;
    st->st_size = 1024;
  } else {
    return -ENOENT;
  }

  return 0;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi) {
  filler(buffer, ".", NULL, 0);  // Current Directory
  filler(buffer, "..", NULL, 0); // Parent Directory

  if (strcmp(path, "/") ==
      0) // If the user is trying to show the files/directories of the root
         // directory show the following
  {
    for (int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++)
      filler(buffer, dir_list[curr_idx], NULL, 0);

    for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
      filler(buffer, files_list[curr_idx], NULL, 0);
  }

  return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset,
                   struct fuse_file_info *fi) {
  int file_idx = get_file_index(path);

  if (file_idx == -1)
    return -1;

  char *content = files_content[file_idx];

  memcpy(buffer, content + offset, size);

  return strlen(content) - offset;
}

static int do_mkdir(const char *path, mode_t mode) {
  path++;
  add_dir(path);

  return 0;
}

static int do_mknod(const char *path, mode_t mode, dev_t rdev) {
  path++;
  add_file(path);

  return 0;
}

static int do_write(const char *path, const char *buffer, size_t size,
                    off_t offset, struct fuse_file_info *info) {
  write_to_file(path, buffer);

  return size;
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,
    .read = do_read,
    .mkdir = do_mkdir,
    .mknod = do_mknod,
    .write = do_write,
};

int main(int argc, char **argv) {
  /*
  initialize();

  bmp_header *bmpHeader = malloc(sizeof(bmp_header));
  bmpHeader->fileType = malloc(sizeof(char) * 3);
  bmpHeader->reserveds = malloc(sizeof(uint16_t) * 2);

  dib_header *dibHeader = malloc(sizeof(dib_header));

  if (argc < 4) {
    printf("Error: BMP path not found in arguments");
    exit(1);
  }

  char *filePath = (char *)argv[3];
  printf("%s\n", filePath);
  printf("\nReading: %s\n", filePath);
  FILE *filePointer = fopen(filePath, "r+");

  if (filePointer == NULL) {
    exitWithError(filePointer, "Error: Opening file");
  }

  getBmpHeader(filePointer, bmpHeader);
  getDibHeader(filePointer, dibHeader);

  fseek(filePointer, bmpHeader->bitmapAddress, SEEK_SET);

  uint32_t bytesInBitMap =
      dibHeader->bmWidth * dibHeader->bmHeight * (dibHeader->bitPerPixel / 8);

  // open and start
  char *option = (char *)argv[4];
  if ((strcmp(option, "start") == 0)) {
    createLsbMethodHeader(filePointer);
    fseek(filePointer, bmpHeader->bitmapAddress, SEEK_SET);
  }

  readLsbMethodHeader(filePointer);

  
  freeMatrixs();
  free(bmpHeader->fileType);
  free(bmpHeader->reserveds);
  free(bmpHeader);
  free(dibHeader);

  fclose(filePointer);
  

    printf("\n\nopaaaa\n\n");

  char **auxArgv;
  argv = malloc(sizeof(*argv) * 3);
  for (int i = 0; i < 3; i++) {
    auxArgv[i] = malloc(sizeof(*auxArgv[i]));
    strcpy( auxArgv[i] , argv[i]);
  }
*/
  return fuse_main(argc, argv, &operations, NULL);
}