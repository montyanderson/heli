#ifndef MAP
#define MAP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct map_entry_s {
	char *key;
	void *value;
};

typedef struct map_s {
	size_t length;
	struct map_entry_s *array;
} Map;

Map *map_create();
void map_set(Map *map, char *key, void *value);
void map_sets(Map *map, char *key, char *value);
void *map_get(Map *map, char *key);
void map_print(Map *map);

#endif
