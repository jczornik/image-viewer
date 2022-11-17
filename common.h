#include <stdio.h>
#include <stdbool.h>

// NOTE: file must be opened before
bool read_bytes_to_buffer(void *to, size_t length, FILE *from);

void change_byte_order(void *to, const void *from, size_t length);
