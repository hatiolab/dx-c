#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep
#include <sys/stat.h> // For stat(), etc.

#include "dx.h"

#include "dx_debug_assert.h"
#include "dx_debug_malloc.h"

#include "dx_util_log.h"

#include "dx_net_packet_file.h"

void net_packet_send_filelist_test();
void net_packet_delete_file_test();
void net_packet_send_file_test();

void net_packet_file_test() {
	net_packet_send_filelist_test();
	net_packet_delete_file_test();
	net_packet_send_file_test();
}

/* 1. 존재하는 디렉토리에서 파일리스트를 읽는다.
 * 2. 존재하지 않는 디렉토리를 파라미터로 주고 파일리스트를 읽는다.(아이템이 없어야 한다.)
 * 3. 디렉토리가 아닌 파일명을 주고 파일리스트를 읽는다.(아이템이 없어야 한다.)
 * 4. 오류의 경우에 header flag에 오류 플래그를 set시켜야 한다.
 */
void net_packet_send_filelist_test() {
	int i;

	/* CASE 1 : 존재하는 디렉토리를 읽는다. */
	dx_fileinfo_array_packet_t* packet = dx_packet_fill_fileinfo_array_packet("/tmp");

	ASSERT("Packet Should not be NULL\n", packet != NULL);

	CONSOLE("Length : %d\n", ntohl(packet->header.len));
	for(i = 0;i < ntohl(packet->array.len);i++) {
		dx_fileinfo_data_t* fileinfo = packet->array.fileinfo + i;
		CONSOLE("Content : %.128s (%d)\n", fileinfo->path, ntohl(fileinfo->size));
	}
	FREE(packet);

	/* CASE 2 : 존재하지 않는 디렉토리를 시도한다. */
	packet = dx_packet_fill_fileinfo_array_packet("/tmp.tmp");
	ASSERT("Packet Should not be NULL\n", packet != NULL);
	ASSERT("File Count Should be 0\n", packet->array.len == 0);

	FREE(packet);

	/* CASE 3 : 존재하는 파일명을 주고 시도한다. */
	packet = dx_packet_fill_fileinfo_array_packet("/home/in/.bash_profile");
	ASSERT("Packet Should not be NULL\n", packet != NULL);
	ASSERT("File Count Should be 0\n", packet->array.len == 0);

	FREE(packet);

	CHKMEM();
}

/* 1. 임시파일을 생성하고, 삭제되는 지를 확인한다.
 * 2. 존재하지 않는 파일명을 주고, 결과를 확인한다.(프로세스가 죽지 않아야 한다.)
 * 3. 폴더명을 주고, 폴더가 삭제되지 않는 것을 확인한다.(파일만 삭제할 수 있어야 한다.)
 */
void net_packet_delete_file_test() {
	char temp[128] = "/tmp/mytemp_XXXXXX\0";
	int tempfd = mkstemp(temp);
	struct stat temp_stat;
	int retval = 0;

	/* CASE 1 : 임시파일을 생성하고, 삭제되는 지를 확인한다. */
	CONSOLE("Temp File Created : %.128s (%d)\n", temp, tempfd);
	unlink(tempfd);

	retval = stat(temp, &temp_stat);
    ASSERT("Tempfile should be found", retval != -1);

	dx_packet_delete_file(0, temp);

	retval = stat(temp, &temp_stat);
    ASSERT("Tempfile should be found", retval == -1);

    /* CASE 2 : 존재하지 않는 파일명을 주고, 결과를 확인한다.(프로세스가 죽지 않아야 한다.) */
	dx_packet_delete_file(0, "/12345");

    /* CASE 3 : 폴더명을 주고, 폴더가 삭제되지 않는 것을 확인한다.(파일만 삭제할 수 있어야 한다.) */
//	dx_packet_delete_file(0, "");

	CHKMEM();
}

/* 1. 존재하는 파일에서 partial을 읽어 오는지 테스트한다.
 * 2. 존재하지 않는 파일을 주고, 결과를 확인한다. (프로세스가 죽지 않아야 한다.)
 * 3. 오류의 경우에 header flag에 오류 플래그를 set시켜야 한다.
 */
void net_packet_send_file_test() {

	CONSOLE("Test Done.\n");

	CHKMEM();
}
