#include "fusemount.h"

// Global variables
char **dir_list;
int16_t curr_dir_idx;

char **files_list;
int16_t curr_file_idx;

char **files_content_list;
int16_t curr_file_content_idx;

bmp_info *bmpInfo;

enum lsbHeaderIndex {

  DIRR_IDX,

  FILE_IDX,

  FILE_CONTENT_IDX
};

// Matrixes handlers

void initializeMatrixes() {
  dir_list = malloc(sizeof(*dir_list) * 10);
  files_list = malloc(sizeof(*files_list) * 10);
  files_content_list = malloc(sizeof(*files_content_list) * 10);

  for (int i = 0; i < 10; i++) {
    dir_list[i] = malloc(sizeof(*(dir_list[i])) * 10);
    files_list[i] = malloc(sizeof(*(files_list[i])) * 10);
    files_content_list[i] = malloc(sizeof(*(files_content_list[i])) * 10);
  }
}

void freeMatrixes() {
  for (int i = 9; i >= 0; i--) {
    free(dir_list[i]);
    free(files_list[i]);
    free(files_content_list[i]);
  }
  free(dir_list);
  free(files_list);
  free(files_content_list);
}

// Aux for save dir names, files names and file contents

void add_dir(const char *dir_name) {
  curr_dir_idx++;
  updateLsbHeader(DIRR_IDX);

  strcpy(dir_list[curr_dir_idx], dir_name);
  uint32_t offset = getIndexValue(DIRR_IDX, curr_dir_idx, bmpInfo->bmpHeader->bitmapAddress);
  fseek(bmpInfo->filePointer, offset, SEEK_SET);
  updateLsbContent(dir_list[curr_dir_idx], bmpInfo->filePointer);
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
  fseek(bmpInfo->filePointer, getIndexValue(FILE_IDX, curr_file_idx, bmpInfo->bmpHeader), SEEK_SET);
  updateLsbContent(files_list[curr_file_idx]);

  curr_file_content_idx++;
  updateLsbHeader(FILE_CONTENT_IDX);

  strcpy(files_content_list[curr_file_content_idx], "\0");
  fseek(bmpInfo->filePointer, getIndexValue(FILE_CONTENT_IDX, curr_file_content_idx, bmpInfo->bmpHeader), SEEK_SET);
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
  fseek(bmpInfo->filePointer, getIndexValue(FILE_CONTENT_IDX, file_idx, bmpInfo->bmpHeader), SEEK_SET);
  updateLsbContent(files_content_list[file_idx]);
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

static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  filler(buffer, ".", NULL, 0);  // Current Directory
  filler(buffer, "..", NULL, 0); // Parent Directory

  if (strcmp(path, "/") == 0) // If the user is trying to show the files/directories of the root
                              // directory show the following
  {
    for (int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++)
      filler(buffer, dir_list[curr_idx], NULL, 0);

    for (int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++)
      filler(buffer, files_list[curr_idx], NULL, 0);
  }

  return 0;
}

static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
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
    if ((strcmp(option, "-s") == 0)) {
      createLsbMethodHeader(bmpInfo->filePointer);
      fseek(bmpInfo->filePointer, bmpInfo->bmpHeader->bitmapAddress, SEEK_SET);
    }
  }

  readLsbMethodHeader(bmpInfo->filePointer);
  readContentInFIle();

  argc = 5;

  printf("\nDirectory starts:\n\n");
  return fuse_main(argc, argv, &operations, NULL);
}