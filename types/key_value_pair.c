#include <stdbool.h>
#include <stdint.h>
#import <string.h>
#import <stdio.h>
#import "../utils/hashmap.h"
#import "key_value_pair.h"

int dict_compare(const void *a, const void *b, void *udata) {
    const KeyValPair *dict_a = a;
    const KeyValPair *dict_b = b;
    return strncmp(dict_a->key, dict_b->key, 127);
}

bool dict_iter(const void *item, void *udata) {
    const KeyValPair *dict = item;
    printf("%s\n", dict->key);
    return true;
}

uint64_t dict_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const KeyValPair *dict = item;
    return hashmap_xxhash3(dict->key, strlen(dict->key), seed0, seed1);
}

const void* get_value_from_key(struct hashmap *map, char *requestedKey) {
    printf("Requested: %s\n", requestedKey);
    const KeyValPair *result = hashmap_get(map, &(KeyValPair){ .key = requestedKey });
    if (result == NULL) { printf("Not found\n"); return NULL; }
    return result->value;
}