#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>		// For lseek, read, close
#include <stdint.h>		// For uint32_t, ...
#include <string.h>
#include <arpa/inet.h>

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

enum AVI_FILE_TYPE {
	AVI_FILE_TYPE_AVI_1_0,
	AVI_FILE_TYPE_OPEN_DML,
	AVI_FILE_TYPE_HYBRID_FILES,
};

#define AVI_LIST_TYPE_RIFF "RIFF"
#define AVI_LIST_TYPE_LIST "LIST"

#define AVI_CONTENT_TYPE_AVI	"AVI "
#define AVI_CONTENT_TYPE_HDR1	"hdrl"
#define AVI_CONTENT_TYPE_INFO	"INFO"
#define AVI_CONTENT_TYPE_MOVI	"MOVI"

#define AVI_CHUNK_TYPE_MOVI		"avih"	/* AVI header */

typedef struct {
	char		cc[4];
	uint32_t	size;
	int8_t		data[0];
} AVI_CHUNK;

typedef struct {
	char		type[4];
	uint32_t	size;
	char		cc[4];
	int8_t		data[0];
} AVI_LIST;

#define DX_AVI_CHUNK_SIZE(sz) (sizeof(AVI_CHUNK) + sz)
#define DX_AVI_LIST_SIZE(sz) (sizeof(AVI_LIST) + sz - 4)

void file_avi_read_header(int fd);
void file_avi_read_info(int fd);
void file_avi_read_idx(int fd);

int dx_avi_riff_handler(int fd, AVI_CHUNK* chunk);
int dx_avi_list_handler(int fd, AVI_CHUNK* chunk);
int dx_avi_chunk_handler(int fd, AVI_CHUNK* chunk);

void file_avi_test() {
	int fd = open("/home/in/1.avi", O_RDONLY);

	AVI_LIST list;
	off_t pos;
	off_t eof_pos;
	int nread;

	pos = read(fd, &list, sizeof(AVI_LIST));
	eof_pos = DX_AVI_LIST_SIZE(list.size);

	ASSERT("AVI List Type should be RIFF\n", strncmp(list.type, AVI_LIST_TYPE_RIFF, strlen(AVI_LIST_TYPE_RIFF)) == 0)
	ASSERT("AVI File Type should be AVI \n", strncmp(list.cc, AVI_CONTENT_TYPE_AVI, strlen(AVI_CONTENT_TYPE_AVI)) == 0)
	printf("Size of RIFF : %d\n\n", list.size);

	while(pos < eof_pos) {
		nread = file_avi_read_chunk(fd);
		if(nread < 0)
			break;
		pos += nread;
		lseek(fd, pos, SEEK_SET);
	}

	printf("\nCurrent File Postion : %d\n", pos);

	CHKMEM();
}

void demo_print_chunk(AVI_CHUNK* chunk) {
	char cc[5];
	memset(cc, 0x0, 5);
	memcpy(cc, chunk->cc, 4);

	printf("CHUNK %4s (%ld bytes)\n", cc, chunk->size);
}

void demo_print_list(AVI_LIST* list) {
	char cc[5], type[5];
	memset(cc, 0x0, 5);
	memset(type, 0x0, 5);

	memcpy(cc, list->cc, 4);
	memcpy(type, list->type, 4);

	printf("%4s %4s (%ld bytes)\n", type, cc, list->size);
}
