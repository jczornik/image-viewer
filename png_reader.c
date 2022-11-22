#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>

#include "common.h"
#include "png_reader.h"

#define FIFTH_BIT_MASK          0x10
#define ANCILLARY_FLAG_MASK     0x1
#define PRIVATE_FLAG_MASK       0x2
#define RESERVED_FLAG_MASK      0x4
#define SAFE_TO_COPY_FLAG_MASK  0x8

#define ANCILLARY_BYTE_INDEX    0
#define PRIVATE_BYTE_INDEX      1
#define RESERVED_BYTE_INDEX     2
#define SAFE_TO_COPY_BYTE_INDEX 3

#define ANCILLARY_FLAG_INDEX    0
#define PRIVATE_FLAG_INDEX      1
#define RESERVED_FLAG_INDEX     2
#define SAFE_TO_COPY_FLAG_INDEX 3

static const uint8_t PNG_MAGIC_NUMBER[] = {137, 80, 78, 71, 13, 10, 26, 10};

/* static struct Chunk_t *create_chunk(uint8_t type[4], char *data, */
/*                                     uint32_t lenght, uint8_t crc[4]) { */
/*   struct Chunk_t *chunk = (struct Chunk_t *)malloc(sizeof(struct Chunk_t)); */

/*   chunk->lenght = lenght; */
/*   chunk->type = (uint8_t *)malloc(sizeof(uint8_t) * chunk->type_lenght); */
/*   chunk->data = (uint8_t *)malloc(sizeof(uint8_t) * chunk->lenght); */
/*   chunk->crc = (uint8_t*)malloc(sizeof(uint8_t) * chunk->crc_length); */

/*   memcpy(chunk->type, type, chunk->type_lenght); */
/*   memcpy(chunk->data, data, chunk->lenght); */
/*   memcpy(chunk->crc, crc, chunk->crc_length); */

/*   return chunk; */
/* } */

static uint8_t extract_flags_from_type(const uint8_t *type) {
  uint8_t flags = ((type[ANCILLARY_BYTE_INDEX] & FIFTH_BIT_MASK) >> (4 - ANCILLARY_FLAG_INDEX)) |
                  ((type[PRIVATE_BYTE_INDEX] & FIFTH_BIT_MASK) >> (4 - PRIVATE_FLAG_INDEX)) |
                  ((type[RESERVED_BYTE_INDEX] & FIFTH_BIT_MASK) >> (4 - RESERVED_FLAG_INDEX)) |
                  ((type[SAFE_TO_COPY_BYTE_INDEX] & FIFTH_BIT_MASK) >> (4 - SAFE_TO_COPY_FLAG_INDEX));

  return flags;
}

// Creates new chunk by moving type, data, and crc
static struct chunk_t *create_chunk_by_move(uint32_t length, uint8_t **data,
                                            uint8_t **type, uint8_t **crc) {
  struct chunk_t *chunk = (struct chunk_t *)malloc(sizeof(struct chunk_t));
  if(!chunk) {
    fprintf(stderr, "Cannot allocate memory for chunk\n");
    return NULL;
  }

  chunk->lenght = length;
  chunk->type = *type;
  chunk->data = *data;
  chunk->crc = *crc;

  chunk->flags = extract_flags_from_type(chunk->type);

  // TODO: run crc check

  *type = NULL;
  *data = NULL;
  *crc = NULL;

  return chunk;
}

void free_chunk_fields(struct chunk_t *chunk) {
  free(chunk->type);
  free(chunk->data);
  free(chunk->crc);
}

void free_chunk(struct chunk_t **chunk) {
  free_chunk_fields(*chunk);
  free(*chunk);
  *chunk = NULL;
}

bool check_magic_number(FILE *img_file) {
  assert(img_file != NULL);

  uint8_t magic_number[sizeof(PNG_MAGIC_NUMBER)];
  if (!read_bytes_to_buffer(magic_number, sizeof(magic_number), img_file)) {
    fprintf(stderr, "Could not read the magic number from file\n");
    return false;
  }
  return memcmp(magic_number, PNG_MAGIC_NUMBER, sizeof(PNG_MAGIC_NUMBER)) == 0;
}

struct chunk_t* read_chunk(FILE *img_file) {
  struct chunk_t *chunk = NULL;
  uint8_t length_buffer[4];
  uint8_t *type = (uint8_t *)malloc(chunk_type_lenght * sizeof(uint8_t));
  uint8_t *data = NULL;
  uint8_t *crc = (uint8_t *)malloc(chunk_crc_length * sizeof(uint8_t));

  if(!type || !crc) {
    fprintf(stderr, "Cannot allocate memory for chunk type or chunk crc\n");
    goto error;
  }

  if(!read_bytes_to_buffer(length_buffer, sizeof(length_buffer), img_file)) {
    fprintf(stderr, "Error while reading chunk length\n");
    goto error;
  }
  change_byte_order(length_buffer, length_buffer, sizeof(length_buffer));

  uint32_t length = *((uint32_t*)length_buffer);
  data = (uint8_t*)malloc(length * sizeof(uint8_t));
  if(!data) {
    fprintf(stderr, "Cannot allocate memory for chunk data\n");
    goto error;
  }

  if(!read_bytes_to_buffer(type, chunk_type_lenght, img_file)) {
    fprintf(stderr, "Cannot read chunk type\n");
    goto error;
  }

  if(!read_bytes_to_buffer(data, length, img_file)) {
    fprintf(stderr, "Cannot read chunk data\n");
    goto error;
  }

  if(!read_bytes_to_buffer(crc, chunk_crc_length, img_file)) {
    fprintf(stderr, "Cannot read chunk crc\n");
    goto error;
  }

  return create_chunk_by_move(length, &data, &type, &crc);

 error:
  free(data);
  free(type);
  free(crc);
  free(chunk);
  return NULL;
}

enum ancillary_flag get_ancillary_flag(const struct chunk_t *chunk) {
  if(chunk->flags & ANCILLARY_FLAG_MASK)
    return ANCILLARY;

  return CRITICAL;
}

enum private_flag get_private_flag(const struct chunk_t *chunk) {
  if(chunk->flags & PRIVATE_FLAG_MASK)
    return PRIVATE;

  return PUBLIC;
}

enum reserved_flag get_reserved_flag(const struct chunk_t *chunk) {
  if(chunk->flags & RESERVED_FLAG_MASK)
    return ERROR;

  return CORRECT;
}

enum safe_to_copy_flag get_safe_to_copy_flag(const struct chunk_t *chunk) {
  if (chunk->flags & SAFE_TO_COPY_FLAG_MASK)
    return SAFE;

  return UNSAFE;
}
