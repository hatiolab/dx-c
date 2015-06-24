#include "demo.h"

dx_console_menu_t* demo_current_menu = NULL;

int demo_console_handler(dx_event_context_t* context) {
	const char* command_exit = "exit";
    char cmdline[128];
    char* remains = NULL;
	dx_console_menu_t* menus = (dx_console_menu_t*)context->pdata;
	dx_console_menu_t* copied;

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

    /* demo exit */
	if(cmdline != NULL && strncmp(cmdline, command_exit, strlen(command_exit)) == 0) {
		quit = 1;
		dx_event_mplexer_wakeup();

		printf("Bye..\n");

		return 0;
	}

	/* find current menu using command line. */
	copied = demo_current_menu;
	demo_current_menu = dx_console_menu_find_menu_by_command(menus, demo_current_menu, cmdline, &remains);

	if(demo_current_menu != NULL && demo_current_menu->handler != NULL) {
		demo_current_menu->handler(remains);
		demo_current_menu = copied; /* 루트로 돌아감 */
	}

	/* 종료되지 않았다면 현재 위치에서의 사용법을 디스플레이 함. */
    if(!quit)
    	dx_print_console_prompt(menus, demo_current_menu);

    return 0;
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
