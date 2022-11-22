#ifndef __PNG_READER
#define __PNG_READER

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

  uint8_t flags;
};

enum ancillary_flag { CRITICAL = 0, ANCILLARY };
enum private_flag { PUBLIC = 0, PRIVATE };
enum reserved_flag { CORRECT = 0, ERROR };
enum safe_to_copy_flag { UNSAFE, SAFE };

bool check_magic_number(FILE *img_file);

void free_chunk_fields(struct chunk_t *chunk);

void free_chunk(struct chunk_t **chunk);

struct chunk_t *read_chunk(FILE *img_file);

enum ancillary_flag get_ancillary_flag(const struct chunk_t *chunk);

enum private_flag get_private_flag(const struct chunk_t *chunk);

enum reserved_flag get_reserved_flag(const struct chunk_t *chunk);

enum safe_to_copy_flag get_safe_to_copy_flag(const struct chunk_t *chunk);

#endif
