#include "map.h"

Map *map_create() {
	Map *map = calloc(1, sizeof(Map));

	return map;
}

void map_set(Map *map, char *key, void *value) {
	for(size_t i = 0; i < map->length; i++) {
		if(strcmp(map->array[i].key, key) == 0) {
			free(map->array[i].value);
			map->array[i].value = value;

			return;
		}
	}

	size_t index = map->length++;
	map->array = realloc(map->array, map->length * sizeof(struct map_entry_s));

	struct map_entry_s entry = {
		.key = strdup(key),
		.value = value
	};

	map->array[index] = entry;
}

void map_sets(Map *map, char *key, char *value) {
	map_set(map, key, strdup(value));
}

void *map_get(Map *map, char *key) {
	for(size_t i = 0; i < map->length; i++) {
		if(strcmp(map->array[i].key, key) == 0) {
			return map->array[i].value;
		}
	}

	return NULL;
}

void map_print(Map *map) {
	for(size_t i = 0; i < map->length; i++) {
		printf("%s=%s\n", map->array[i].key, (char *) map->array[i].value);
	}

	printf("\n");
}
