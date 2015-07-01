// Copyright (c) 2015 - 2015 All Right Reserved, http://hatiolab.com
//
// This source is subject to the Hatio, Lab. Permissive License.
// Please see the License.txt file for more information.
// All other rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WIHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>
#include <linux/uvcvideo.h>
#include <linux/usb/video.h>

#include "dx_util_file.h"


typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} io_method;

#define HEX_DECODE_CHAR(c)	((c) >= '0' && (c) <= '9' ? (c) - '0' : (tolower(c)) - 'a' + 0xA)
#define HEX_DECODE_BYTE(cc)	((HEX_DECODE_CHAR((cc)[0]) << 4) + HEX_DECODE_CHAR((cc)[1]))

void init_device(int fd, int io) {
//	struct v4l2_capability cap;
//	struct v4l2_cropcap cropcap;
//	struct v4l2_crop crop;
//	struct v4l2_format fmt;
//
//	unsigned int min;
//	unsigned char view_mode = 0x0A; /* view mode. ie. TOP_VIEW */
//
//	struct uvc_xu_control_mapping control_mapping = { .id = V4L2_CID_LASTP1,
//			.name = "view_mode",
//			.entity = { 0x08, 0x94, 0x00, 0xC2, 0x01, 0x1F, 0x44, 0xA5, 0xA7,
//					0x90, 0x75, 0xFC, 0x76, 0xB6, 0x25, 0x00 },
//			.selector = 0x01, .size = 16, /* in bits */
//			.offset = 0, /* in bits */
//			.v4l2_type = V4L2_CTRL_TYPE_INTEGER, .data_type =
//					UVC_CTRL_DATA_TYPE_UNSIGNED, };
//
//	struct uvc_xu_control_query control_query;
//
//	int ret;
//
//	ret = ioctl(fd, UVCIOC_CTRL_MAP, &control_mapping);
//
//	if (ret < 0) {
//		int err = errno;
//		printf("ERROR: %d, --------------\n", err);
//
//		switch (err) {
//		case ENOENT:
//			printf("NOENT\n");
//			break;
//		case ENOMEM:
//			printf("NOMEM\n");
//			break;
//		case EPERM:
//			printf("EPERM\n");
//			break;
//		case EINVAL:
//			printf("EINVAL\n");
//			break;
//		case EOVERFLOW:
//			printf("EOVERFLOW\n");
//			break;
//		case EEXIST:
//			printf("EEXIST\n");
//			break;
//		}
//	}
//
//	if (-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)) {
//		if (EINVAL == errno) {
//			fprintf(stderr, "%s is no V4L2 device\n", dx_file_get_path(fd));
//			exit(EXIT_FAILURE);
//		} else {
//			errno_exit("VIDIOC_QUERYCAP");
//		}
//	}
//
//	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
//		fprintf(stderr, "%s is no video capture device\n", dx_file_get_path(fd));
//		exit(EXIT_FAILURE);
//	}
//
//	switch (io) {
//	case IO_METHOD_READ:
//		if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
//			fprintf(stderr, "%s does not support read i/o\n", dx_file_get_path(fd));
//			exit(EXIT_FAILURE);
//		}
//		break;
//
//	case IO_METHOD_MMAP:
//	case IO_METHOD_USERPTR:
//		if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
//			fprintf(stderr, "%s does not support streaming i/o\n", dx_file_get_path(fd));
//			exit(EXIT_FAILURE);
//		}
//		break;
//	}
//
//	/* Select video input, video standard and tune here. */
//
//	CLEAR(cropcap);
//
//	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//
//	if (0 == ioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
//		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//		crop.c = cropcap.defrect; /* reset to default */
//
//		if (-1 == ioctl(fd, VIDIOC_S_CROP, &crop)) {
//			switch (errno) {
//			case EINVAL:
//				/* Cropping not supported. */
//				break;
//			default:
//				/* Errors ignored. */
//				break;
//			}
//		}
//	} else {
//		/* Errors ignored. */
//		fprintf(stderr, "%s does not support CROPCAP\n", dx_file_get_path(fd));
//	}
//
//	CLEAR(fmt);
//
//	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
////	if (p->force_format) {
//	if (1) {
//		fmt.fmt.pix.width = 1280;
//		fmt.fmt.pix.height = 720;
////		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
//		fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
//
//		if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
//			errno_exit("VIDIOC_S_FMT");
//
//		/* Note VIDIOC_S_FMT may change width and height. */
//	} else {
//		/* Preserve original settings as set by v4l2-ctl for example */
//		if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
//			errno_exit("VIDIOC_G_FMT");
//	}
//
//	/* Buggy driver paranoia. */
//	min = fmt.fmt.pix.width * 2;
//	if (fmt.fmt.pix.bytesperline < min)
//		fmt.fmt.pix.bytesperline = min;
//	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
//	if (fmt.fmt.pix.sizeimage < min)
//		fmt.fmt.pix.sizeimage = min;
//
//	printf("bytesperline : %d, sizeimage : %d\n", fmt.fmt.pix.bytesperline,
//			fmt.fmt.pix.sizeimage);
//
//	switch (io) {
//	case IO_METHOD_READ:
//		init_read(p, fmt.fmt.pix.sizeimage);
//		break;
//
//	case IO_METHOD_MMAP:
//		init_mmap(p);
//		break;
//
//	case IO_METHOD_USERPTR:
//		init_userp(p, fmt.fmt.pix.sizeimage);
//		break;
//	}
//
//	CLEAR(control_query);
//
//	control_query.unit = 2; /* Extension Unit ID */
//	control_query.selector = 1; /* Control Selector */
//	control_query.query = UVC_SET_CUR;
//	control_query.size = 2; /* Data Length */
//	control_query.data = &view_mode;
//
//	if (-1 == ioctl(fd, UVCIOC_CTRL_QUERY, &control_query)) {
//		errno_exit("UVCIOC_CTRL_QUERY");
//	}
}

void uninit_device(int fd) {
//	unsigned int i;
//
//	switch (p->io) {
//	case IO_METHOD_READ:
//		free(p->buffers[0].start);
//		break;
//
//	case IO_METHOD_MMAP:
//		for (i = 0; i < p->n_buffers; ++i)
//			if (-1 == munmap(p->buffers[i].start, p->buffers[i].length))
//				errno_exit("munmap");
//		break;
//
//	case IO_METHOD_USERPTR:
//		for (i = 0; i < p->n_buffers; ++i)
//			free(p->buffers[i].start);
//		break;
//	}
//
//	free(p->buffers);
}

int dx_v4l2_open_device(char* dev_name) {
	struct stat st;
	int fd;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	fd = open(dev_name, O_RDWR /* required */| O_NONBLOCK, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno,
				strerror(errno));
		exit(EXIT_FAILURE);
	}

	return 0;
}

