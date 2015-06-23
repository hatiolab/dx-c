#include "demo.h"

int dx_server = -1;
int dx_client = -1;

int quit = 0;

int demo_console_handler(dx_event_context_t* context);

int main() {

	dx_event_mplexer_create();

	dx_console_start(demo_console_handler);

	/* Big Loop */
	while(!quit) {
		dx_event_mplexer_poll(-1);
	}

	dx_event_mplexer_destroy();

	CHKMEM();

	return 0;
}
