#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

bool read_bytes_to_buffer(void *to, size_t length, FILE *from) {
  if(length == 0)
    return true;

  size_t read = fread(to, length, 1, from);

  if (read != 1) {
    if (ferror(from)) {
      fprintf(stderr, "Error reading buffer: %s\n", strerror(errno));
      return false;
    } else if (feof(from)) {
      fprintf(stderr, "Error reading buffer - EOF reached\n");
      return false;
    } else {
      assert(0 && "unreachable");
    }
  }

  return true;
}

void change_byte_order(void *to, const void *from, size_t length) {
  const uint8_t *ptr_from = (const uint8_t *)from;
  uint8_t *ptr_to = (uint8_t *)to;
  uint8_t tmp;

  for (size_t i = 0; i < length / 2; ++i) {
    tmp = ptr_from[i];
    ptr_to[i] = ptr_from[length - i - 1];
    ptr_to[length - i - 1] = tmp;
  }
}
