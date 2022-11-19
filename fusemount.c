// Desenvolvido por Rafael Nascimento Lourencobmp_header
// Referencias
// https://docs.fileformat.com/image/bmp/
// https://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure
// https://en.wikipedia.org/wiki/BMP_file_format
// https://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/

#include "fusemount.h"
#include "lsb.h"

// Global variables
// Matrixes
char **dirArray;
char **filesArray;
char **filesContentArray;

// Indexes header
int16_t dirIndex;
int16_t fileIndex;
int16_t fileContentIndex;

bmp_info *bmpInfo;

enum lsbHeaderIndex { DIRR_IDX, FILE_IDX, FILE_CONTENT_IDX };

// Matrixes handlers

void initializeMatrixes() {
  dirArray = malloc(sizeof(*dirArray) * MATRIX_LENGHT);
  filesArray = malloc(sizeof(*filesArray) * MATRIX_LENGHT);
  filesContentArray = malloc(sizeof(*filesContentArray) * MATRIX_LENGHT);

  for (int i = 0; i < MATRIX_LENGHT; i++) {
    dirArray[i] = malloc(sizeof(*(dirArray[i])) * MATRIX_LENGHT);
    filesArray[i] = malloc(sizeof(*(filesArray[i])) * MATRIX_LENGHT);
    filesContentArray[i] = malloc(sizeof(*(filesContentArray[i])) * MATRIX_LENGHT);
  }
}

void freeMatrixes() {
  for (int i = (MATRIX_LENGHT - 1); i >= 0; i--) {
    free(dirArray[i]);
    free(filesArray[i]);
    free(filesContentArray[i]);
  }
  free(dirArray);
  free(filesArray);
  free(filesContentArray);
}

// Aux for save dir names, files names and file contents

void add_dir(const char *dir_name) {
  dirIndex++;
  setIndexHeaderInFile(bmpInfo->filePointer, DIRR_IDX, dirIndex, bmpInfo->bmpHeader->bitmapAddress);

  strcpy(dirArray[dirIndex], dir_name);
  uint32_t offset = getIndexOfBmpWithLsb(DIRR_IDX, dirIndex, bmpInfo->bmpHeader->bitmapAddress);
  fseek(bmpInfo->filePointer, offset, SEEK_SET);
  setMatrixContentInFile(dirArray[dirIndex], bmpInfo->filePointer);
}

int is_dir(const char *path) {
  path++; // Eliminating "/" in the path

  for (int curr_idx = 0; curr_idx <= dirIndex; curr_idx++)
    if (strcmp(path, dirArray[curr_idx]) == 0)
      return 1;

  return 0;
}

void add_file(const char *filename) {
  uint32_t offset;
  fileIndex++;
  setIndexHeaderInFile(FILE_IDX);

  strcpy(filesArray[fileIndex], filename);
  offset = getIndexOfBmpWithLsb(FILE_IDX, fileIndex, bmpInfo->bmpHeader->bitmapAddress);
  fseek(bmpInfo->filePointer, offset, SEEK_SET);
  setMatrixContentInFile(filesArray[fileIndex], bmpInfo->filePointer);

  fileContentIndex++;
  setIndexHeaderInFile(FILE_CONTENT_IDX);

  strcpy(filesContentArray[fileContentIndex], "\0");
  offset = getIndexOfBmpWithLsb(FILE_CONTENT_IDX, fileContentIndex, bmpInfo->bmpHeader->bitmapAddress);
  fseek(bmpInfo->filePointer, offset, SEEK_SET);
  setMatrixContentInFile(filesContentArray[fileContentIndex]);
}

int is_file(const char *path) {
  path++; // Eliminating "/" in the path

  for (int curr_idx = 0; curr_idx <= fileIndex; curr_idx++)
    if (strcmp(path, filesArray[curr_idx]) == 0)
      return 1;

  return 0;
}

int get_file_index(const char *path) {
  path++; // Eliminating "/" in the path

  for (int curr_idx = 0; curr_idx <= fileIndex; curr_idx++)
    if (strcmp(path, filesArray[curr_idx]) == 0)
      return curr_idx;

  return -1;
}

void write_to_file(const char *path, const char *new_content) {
  int file_idx = get_file_index(path);

  if (file_idx == -1) // No such file
    return;

  strcpy(filesContentArray[file_idx], new_content);
  uint32_t offset = getIndexOfBmpWithLsb(DIRR_IDX, dirIndex, bmpInfo->bmpHeader->bitmapAddress);
  fseek(bmpInfo->filePointer, offset, SEEK_SET);
  setMatrixContentInFile(filesContentArray[file_idx]);
}

// Fuser methods to be used

static int do_getattr(const char *path, struct stat *s) {
  s->st_uid = getuid();     // The owner of the file/directory is the user who
                            // mounted the filesystem
  s->st_gid = getgid();     // The group of the file/directory is the same as the
                            // group of the user who mounted the filesystem
  s->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
  s->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now

  if (strcmp(path, "/") == 0 || is_dir(path) == 1) {
    s->st_mode = S_IFDIR | 0755;
    s->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is
                     // here: http://unix.stackexchange.com/a/MATRIX_LENGHT1536
  } else if (is_file(path) == 1) {
    s->st_mode = S_IFREG | 0644;
    s->st_nlink = 1;
    s->st_size = 1024;
  } else {
    return -ENOENT;
  }

  return 0;
}

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  filler(buffer, ".", NULL, 0);  // Current Directory
  filler(buffer, "..", NULL, 0); // Parent Directory

  if (strcmp(path, "/") == 0) // If the user is trying to show the files/directories of the root
                              // directory show the following
  {
    for (int curr_idx = 0; curr_idx <= dirIndex; curr_idx++)
      filler(buffer, dirArray[curr_idx], NULL, 0);

    for (int curr_idx = 0; curr_idx <= fileIndex; curr_idx++)
      filler(buffer, filesArray[curr_idx], NULL, 0);
  }

  return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
  int file_idx = get_file_index(path);

  if (file_idx == -1)
    return -1;

  char *content = filesContentArray[file_idx];

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

static int do_write(const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info) {
  write_to_file(path, buffer);

  return size;
}

static void do_destroy() {
  printf("\nDestroying mallocs\n");
  freeMatrixes();
  free(bmpInfo->bmpHeader->fileType);
  free(bmpInfo->bmpHeader->reserveds);
  free(bmpInfo->bmpHeader);
  free(bmpInfo->dibHeader);
  fclose(bmpInfo->filePointer);
  free(bmpInfo);
}

static struct fuse_operations operations = {.getattr = do_getattr,
                                            .readdir = do_readdir,
                                            .read = do_read,

                                            .mkdir = do_mkdir,

                                            .mknod = do_mknod,
                                            .write = do_write,

                                            .destroy = do_destroy};

int main(int argc, char **argv) {
  initializeMatrixes();
  bmpInfo = initializeBmpInfo(argc, argv);

  // save a default value at image
  if (argc == 7) {
    char *option = (char *)argv[6];
    if ((strcmp(option, "init") == 0)) {
      createIndexesHeaderInFile(bmpInfo->filePointer);
      fseek(bmpInfo->filePointer, bmpInfo->bmpHeader->bitmapAddress, SEEK_SET);
    }
  }

  setIndexHeaderPointer(&dirIndex, bmpInfo->filePointer);
  setIndexHeaderPointer(&fileIndex, bmpInfo->filePointer);
  setIndexHeaderPointer(&fileContentIndex, bmpInfo->filePointer);

  readMatrixContentInFIle();
  readMatrixContentInFIle();
  readMatrixContentInFIle();

  argc = 5;

  printf("\nDirectory starts:\n\n");
  return fuse_main(argc, argv, &operations, NULL);
}