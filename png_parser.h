#ifndef __PNG_PARSER
#define __PNG_PARSER

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

static const uint8_t chunk_type_lenght = 4;
static const uint8_t chunk_crc_length = 4;

struct chunk_t {
  uint32_t lenght;
  uint8_t *type;
  uint8_t *data;
  uint8_t *crc;
};

bool check_magic_number(FILE *img_file);

void free_chunk_fields(struct chunk_t *chunk);

void free_chunk(struct chunk_t **chunk);

struct chunk_t* read_chunk(FILE *img_file);

#endif
