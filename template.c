#include "template.h"

Template *template_create() {
	Template *template = calloc(1, sizeof(Template));

	return template;
}

void template_append(Template *template, int type, char *content) {
	struct template_entry_s entry = {
		.type = type,
		.content = strdup(content)
	};

	size_t index = template->length++;
	template->array = realloc(template->array, template->length * sizeof(struct template_entry_s));
	template->array[index] = entry;
}

Template *template_parse(char *input) {
	Template *template = template_create();

	size_t start = 0;

	for(size_t i = 0; i < 1 || input[i - 1] != 0; i++) {
		if(input[i] == '{' || input[i] == 0) {
			char *key = calloc((i - start) + 1, 1);
			memcpy(key, &input[start], i - start);

			template_append(template, TEMPLATE_TEXT, key);
			free(key);

			start = i + 1;
		}

		if(input[i] == '}') {
			char *key = calloc((i - start) + 1, 1);
			memcpy(key, &input[start], i - start);

			template_append(template, TEMPLATE_VARIABLE, key);
			free(key);

			start = i + 1;
		}
	}

	return template;
}

char *template_render(Template *template, Map *map) {
	char *output = NULL;
	size_t output_length = 0;

	for(size_t i = 0; i < template->length; i++) {
		struct template_entry_s entry = template->array[i];

		size_t index = output_length;

		if(entry.type == TEMPLATE_TEXT) {
			size_t length = strlen(entry.content);

			output_length += length;
			output = realloc(output, output_length);

			memcpy(&output[index], entry.content, length);
		}

		if(entry.type == TEMPLATE_VARIABLE) {
			char *value = map_get(map, entry.content);

			size_t length = strlen(value);

			output_length += length;
			output = realloc(output, output_length);

			memcpy(&output[index], value, length);
		}
	}

	return output;
}

void template_print(Template *template) {
	printf("printing %lu\n", template->length);

	for(size_t i = 0; i < template->length; i++) {
		printf("'%s' %d\n", template->array[i].content, template->array[i].type);
	}
}
