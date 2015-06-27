#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>		// For lseek, read, close
#include <stdint.h>		// For uint32_t, ...
#include <string.h>
#include <arpa/inet.h>

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_file_movie.h"
#include "dx_file_avi.h"

#define AVI_LIST_TYPE_RIFF "RIFF"
#define AVI_LIST_TYPE_LIST "LIST"

/*
 * 1. 파일을 연다.
 * 2. 탑 리스트를 읽는다.
 * 3. 헤더, 무비, 인덱스 오프셋을 구한다.
 * 4. 헤더 리스트를 구한다.
 */

#define AVI_CONTENT_TYPE_AVI	"AVI "
#define AVI_CONTENT_TYPE_HDR1	"hdrl"
#define AVI_CONTENT_TYPE_INFO	"INFO"
#define AVI_CONTENT_TYPE_MOVI	"MOVI"

typedef int (*dx_avi_scheme_handler)(int fd, dx_avi_chunk_t* chunk);
typedef struct dx_avi_chunk_map {
	char	type[4];
	dx_avi_scheme_handler handler;
} dx_avi_chunk_map_t;

void file_avi_open_test(char* path, dx_movie_context_t* movie) {
	dx_movie_context_t context;

	dx_avi_list_t list;
	int fd;

	off_t pos;
	off_t eof;
	int nread;

	/*
	 * 파일을 오픈한다.
	 */
	fd = open("/home/in/1.avi", O_RDONLY);

	/*
	 * 탑리스트를 읽는다.
	 */

	pos = read(fd, &list, sizeof(dx_avi_list_t));
	eof = DX_AVI_LIST_SIZE(list.size);

	ASSERT("AVI List Type should be RIFF\n", strncmp(list.type, AVI_LIST_TYPE_RIFF, strlen(AVI_LIST_TYPE_RIFF)) == 0)
	ASSERT("AVI File Type should be AVI \n", strncmp(list.cc, AVI_CONTENT_TYPE_AVI, strlen(AVI_CONTENT_TYPE_AVI)) == 0)
	CONSOLE("Size of RIFF : %d\n\n", list.size);

	while(pos < eof) {
		nread = file_avi_read_chunk(fd);
		if(nread < 0)
			break;
		pos += nread;
		lseek(fd, pos, SEEK_SET);
	}

	CONSOLE("\nCurrent File Postion : %d\n", pos);

}
