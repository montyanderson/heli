#include "router.h"
#include <pthread.h>

Request *request_create() {
	Request *request = calloc(1, sizeof(Request));

	request->locals = map_create();

	return request;
}

void request_write(Request *request, char *data) {
	if(request->headers_sent == 0) {
		char *headers = "HTTP/1.0 200 OK\nConnection: close\n\n";

		send(request->fd, headers, strlen(headers), 0);

		request->headers_sent = 1;
	}

	send(request->fd, data, strlen(data), 0);
}

void request_render(Request *request, char *filename) {
	FILE *f = fopen(filename, "rb");

	fseek(f, 0, SEEK_END);

	size_t length = ftell(f);
	char *input = calloc(length, 1);

	fseek(f, 0, SEEK_SET);
	fread(input, 1, length, f);

	fclose(f);

	Template *template = template_parse(input);

	char *output = template_render(template, request->locals);

	request_write(request, output);
	request_write(request, "\n\n");
}

Router *router_create() {
	Router *router = calloc(1, sizeof(Router));

	router->routes = map_create();

	return router;
}

void router_get(Router *router, char *path, void (*handler)(Request *)) {
	map_set(router->routes, path, handler);
}

void router_run(Router *router, char *path) {

}

void error(char *reason) {
	perror(reason);
	exit(1);
}

struct router_thread_s {
	Router *router;
	Request *request;
};

void *router_thread(void *_data) {
	struct router_thread_s *data = _data;

	Router *router = data->router;
	Request *request = data->request;

	size_t buffer_length = 1024 * 1024;
	char *buffer = calloc(buffer_length, 1);

	size_t index = 0;

	while(1) {
		ssize_t r = recv(request->fd, buffer + index, buffer_length - index, 0);

		if(r > 0) {
			index += r;
		} else {
			error("recv");
		}

		int end = 0;

		for(size_t i = 3; i < index; i++) {
			if(buffer[i] == '\n' && (buffer[i - 1] == '\n' || buffer[i - 2] == '\n')) {
				end = 1;
			}
		}

		if(end == 1) {
			break;
		}
	}

	size_t part = 0;
	size_t start = 0;

	char *method;
	char *path;

	printf("%s\n", buffer);

	for(size_t i = 0; i < index; i++) {
		if(buffer[i] == ' ' || buffer[i] == '\n') {
			if(part == 0) {
				method = calloc(i - start + 1, 1);
				memcpy(method, &buffer[start], i - start);

				start = i + 1;
				part++;
			} else if(part == 1) {
				path = calloc(i - start + 1, 1);
				memcpy(path, &buffer[start], i - start);

				start = i + 1;
				part++;

				break;
			}
		}
	}

	printf("working\n");

	void (*handler)(Request *);

	handler = map_get(router->routes, path);

	if(handler == NULL) {
		request_write(request, "HTTP/1.0 400 Not Found\nConnection: close\n\n");
	} else {
		handler(request);
	}

	close(request->fd);

	free(buffer);
	free(method);
	free(path);
}

void router_listen(Router *router, int port) {
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd < 0)
		error("socket");

	int on = 1;

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		error("setsockopt");
	}

	struct sockaddr_in servaddr;

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if(bind(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		error("bind");
	}

	if(listen(fd, SOMAXCONN) < 0) {
		error("listen");
	}

	while(1) {
		int connfd = accept(fd, (struct sockaddr *) NULL, NULL);

		Request *request = request_create();

		request->fd = connfd;

		pthread_t thread;

		struct router_thread_s *data = calloc(1, sizeof(struct router_thread_s));

		data->router = router;
		data->request = request;

		pthread_create(&thread, NULL, router_thread, data);
	}
}
