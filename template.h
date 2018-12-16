#ifndef TEMPLATE
#define TEMPLATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"

#define TEMPLATE_TEXT 1
#define TEMPLATE_VARIABLE 2

struct template_entry_s {
	char *content;
	int type;
};

typedef struct template_s {
	size_t length;
	struct template_entry_s *array;
} Template;

Template *template_create();
void template_append(Template *template, int type, char *content);
Template *template_parse(char *input);
char *template_render(Template *template, Map *map);
void template_print(Template *template);

#endif
