#include "router.h"

/* App */

void _index(Request *req) {
	map_sets(req->locals, "name", "Monty");

	request_render(req, "index.tpl");
}

int main() {
	Router *router = router_create();

	router_get(router, "/", _index);

	router_listen(router, 3000);
}
