#ifndef ROUTER
#define ROUTER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <endian.h>
#include <fcntl.h>
#include <unistd.h>

#include "map.h"
#include "template.h"

typedef struct router_s {
	Map *routes;
} Router;

typedef struct request_s {
	int fd;
	int headers_sent;
	Map *locals;
} Request;

Request *request_create();
void request_write(Request *request, char *data);
void request_render(Request *request, char *filename);
Router *router_create();
void router_get(Router *router, char *path, void (*handler)(Request *));
void router_run(Router *router, char *path);
void router_listen(Router *router, int port);

#endif
