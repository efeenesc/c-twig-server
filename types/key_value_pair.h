#ifndef key_value_pair_h
#define key_value_pair_h

#include <stdbool.h>
#include <stdint.h>
#include "../utils/hashmap.h"

typedef struct {
  char *key;
  char *value;
} KeyValPair;

int dict_compare(const void *a, const void *b, void *udata);
bool dict_iter(const void *item, void *udata);
uint64_t dict_hash(const void *item, uint64_t seed0, uint64_t seed1);
const void* get_value_from_key(struct hashmap *map, char *requestedKey);

#endif