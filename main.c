#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png_reader.h"

int main(int argc, char **argv) {
  (void)argc;
  assert(*argv != NULL);

  char *program = *argv++;
  char *img_path = *argv++;

  if (img_path == NULL) {
    fprintf(stderr,
            "Usage %s <filepath>\n"
            "There is no file path provided.\n",
            program);
    exit(1);
  }

  FILE *img_file = fopen(img_path, "rb");

  if (img_file == NULL) {
    fprintf(stderr, "Error opening file %s: %s\n", img_path, strerror(errno));
    exit(1);
  }

  if(!check_magic_number(img_file)) {
    fprintf(stderr, "File \"%s\" does not contain correct magic number\n", img_path);
    exit(1);
  }

  printf("Magic number is correct!\n");

  struct chunk_t *chunk = NULL;
  while((chunk = read_chunk(img_file))) {
    printf("Reading chunk successful\n");
    printf("Chunk data length: %u\n", chunk->lenght);
    free_chunk(&chunk);
  }
  fclose(img_file);

  return 0;
}
