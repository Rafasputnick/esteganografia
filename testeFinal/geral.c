// Desenvolvido por Rafael Nascimento Lourencobmp_header
// Referencias
// https://docs.fileformat.com/image/bmp/
// https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure
// https://en.wikipedia.org/wiki/BMP_file_format
// https://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/
#define FUSE_USE_VERSION 30

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




















/////////////////////////////////////////////////////////////////////

// Trabalhando com o diretorio (libfuse)

void add_dir(const char *dir_name) {
  curr_dir_idx++;
  updateLsbHeader(DIRR_IDX);

  strcpy(dir_list[curr_dir_idx], dir_name);
  fseek(filePointer, getIndexValue(DIRR_IDX, curr_dir_idx), SEEK_SET);
  updateLsbContent(dir_list[curr_dir_idx]);
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
  updateLsbHeader(FILE_IDX);

  strcpy(files_list[curr_file_idx], filename);
  fseek(filePointer, getIndexValue(FILE_IDX, curr_file_idx), SEEK_SET);
  updateLsbContent(files_list[curr_file_idx]);

  curr_file_content_idx++;
  updateLsbHeader(FILE_CONTENT_IDX);

  strcpy(files_content_list[curr_file_content_idx], "\0");
  fseek(filePointer, getIndexValue(FILE_CONTENT_IDX, curr_file_content_idx),
        SEEK_SET);
  updateLsbContent(files_content_list[curr_file_content_idx]);
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

  strcpy(files_content_list[file_idx], new_content);
  fseek(filePointer, getIndexValue(FILE_CONTENT_IDX, file_idx), SEEK_SET);
  updateLsbContent(files_content_list[file_idx]);
}

static int do_getattr(const char *path, struct stat *s) {
  s->st_uid = getuid(); // The owner of the file/directory is the user who
                        // mounted the filesystem
  s->st_gid = getgid(); // The group of the file/directory is the same as the
                        // group of the user who mounted the filesystem
  s->st_atime =
      time(NULL); // The last "a"ccess of the file/directory is right now
  s->st_mtime =
      time(NULL); // The last "m"odification of the file/directory is right now

  if (strcmp(path, "/") == 0 || is_dir(path) == 1) {
    s->st_mode = S_IFDIR | 0755;
    s->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is
                     // here: http://unix.stackexchange.com/a/101536
  } else if (is_file(path) == 1) {
    s->st_mode = S_IFREG | 0644;
    s->st_nlink = 1;
    s->st_size = 1024;
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

  char *content = files_content_list[file_idx];

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

static void do_destroy() {
  printf("\nDestroying mallocs\n");
  freeMatrixs();
  free(bmpHeader->fileType);
  free(bmpHeader->reserveds);
  free(bmpHeader);
  free(dibHeader);

  fclose(filePointer);
}

static struct fuse_operations operations = {.getattr = do_getattr,
                                            .readdir = do_readdir,
                                            .read = do_read,

                                            .mkdir = do_mkdir,

                                            .mknod = do_mknod,
                                            .write = do_write,

                                            .destroy = do_destroy};

int main(int argc, char **argv) {
  initialize();

  bmpHeader = malloc(sizeof(bmp_header));
  bmpHeader->fileType = malloc(sizeof(char) * 3);
  bmpHeader->reserveds = malloc(sizeof(uint16_t) * 2);

  dibHeader = malloc(sizeof(dib_header));

  if (argc < 4) {
    printf("Error: BMP path not found in arguments");
    exit(1);
  }

  char *filePath = (char *)argv[5];
  printf("\nReading: %s\n", filePath);
  filePointer = fopen(filePath, "r+");

  if (filePointer == NULL) {
    exitWithError(filePointer, "Error: Opening file");
  }

  getBmpHeader(filePointer, bmpHeader);
  getDibHeader(filePointer, dibHeader);

  fseek(filePointer, bmpHeader->bitmapAddress, SEEK_SET);

  uint32_t bytesInBitMap =
      dibHeader->bmWidth * dibHeader->bmHeight * (dibHeader->bitPerPixel / 8);

  // open and start
  if (argc == 7) {
    char *option = (char *)argv[6];
    if ((strcmp(option, "-s") == 0)) {
      createLsbMethodHeader(filePointer);
      fseek(filePointer, bmpHeader->bitmapAddress, SEEK_SET);
    }
  }

  readLsbMethodHeader(filePointer);
  readContentInFIle();

  argc = 5;

  printf("\nDirectory starts:\n\n");
  return fuse_main(argc, argv, &operations, NULL);
}