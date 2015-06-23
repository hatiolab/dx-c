#include "demo.h"

demo_menu_asso_t top_menu_map[] = {
	{"start", demo_console_start_handler, start_menu_map},
	{"discovery", demo_console_start_handler, NULL},
	{"server", demo_console_start_handler, NULL},
	{"client", demo_console_start_handler, NULL},
};

console_handler demo_current_handler = NULL;
demo_menu_asso_t* demo_current_menu_map = top_menu_map;

console_handler dx_console_find_handler(char* cmdline, demo_menu_asso_t* menu_map) {
	const char* whitespace = " \t\n\f";
	const char* command_exit = "exit";
	int i, sz;
	char* next_token;
	char* command = strtok_r(cmdline, whitespace, &next_token);

	if(strncmp(command, command_exit, 4) == 0) {
		demo_current_menu_map = top_menu_map;
		return NULL;
	}

	for(i = 0;i < 4;i++) {
		command = menu_map[i].command;
		sz = strlen(command) > cmdline ? cmdline : strlen(command);

		if(strncmp(cmdline, command, sz) == 0) {
			if(menu_map[i].menu_map != NULL && next_token != NULL && strlen(next_token) > 0) {

				demo_current_menu_map = menu_map[i].menu_map;
				return dx_console_find_handler(next_token, menu_map[i].menu_map);
			}
			return menu_map[i].handler;
		}
	}
	return NULL;
}

int demo_console_handler(dx_event_context_t* context) {
    char cmdline[128];

    bzero(cmdline, 128);

    ssize_t nbytes = read(context->fd, cmdline, sizeof(cmdline));
    if(0 == nbytes) {
        printf("Console hung up\n");
        return -1;
    } else if(0 > nbytes) {
        perror("Console read() error");
        close(context->fd);
        dx_del_event_context(context);
        return -2;
    }

    demo_current_handler = dx_console_find_handler(cmdline, demo_current_menu_map);

    if(demo_current_handler != NULL) {
    	demo_current_handler(context, cmdline);
    } else {
    	printf("Unknown Command!\n");
    }

    if(!quit)
    	dx_print_console_prompt();

    return 0;
}

void demo_quit() {
	quit = 1;
	dx_event_mplexer_wakeup();
}

int send_stream(int fd) {
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	uint8_t* data = (uint8_t*)MALLOC(800*480*2);

	for(l = 0;l < 100;l++) {
		for(i = 0;i < 480;i++) {
			for(j = 0;j < 800;j++) {
				data[(i * 480 + j) * 2] = (uint8_t)k;
				data[(i * 480 + j) * 2 + 1] = (uint8_t)k++;
			}
		}

		dx_packet_send_stream(fd, DX_STREAM, 0, data, 800*480*2);
	}

	FREE(data);

	return 0;
}
