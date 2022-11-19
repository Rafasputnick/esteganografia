#include "fusemount.h"

char **dir_list;
int16_t curr_dir_idx;

char **files_list;
int16_t curr_file_idx;

char **files_content_list;
int16_t curr_file_content_idx;

enum lsbHeaderIndex {

  DIRR_IDX,

  FILE_IDX,

  FILE_CONTENT_IDX
};

void initialize() {
  dir_list = malloc(sizeof(*dir_list) * 10);
  files_list = malloc(sizeof(*files_list) * 10);
  files_content_list = malloc(sizeof(*files_content_list) * 10);

  for (int i = 0; i < 10; i++) {
    dir_list[i] = malloc(sizeof(*(dir_list[i])) * 10);
    files_list[i] = malloc(sizeof(*(files_list[i])) * 10);
    files_content_list[i] = malloc(sizeof(*(files_content_list[i])) * 10);
  }
}

void freeMatrixs() {
  for (int i = 9; i >= 0; i--) {
    free(dir_list[i]);
    free(files_list[i]);
    free(files_content_list[i]);
  }
  free(dir_list);
  free(files_list);
  free(files_content_list);
}