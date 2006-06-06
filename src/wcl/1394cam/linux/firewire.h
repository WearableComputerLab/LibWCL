/*******************************************************
 *
 * Author: Hirokazu Kato, Atsishi Nakazawa
 *
 *         kato@sys.im.hiroshima-cu.ac.jp
 *         nakazawa@inolab.sys.es.osaka-u.ac.jp
 *
 * Revision: 4.3
 * Date: 2002/01/01
 *
 *******************************************************/
#ifndef AR_VIDEO_H
#define AR_VIDEO_H

#include <stdlib.h>
#include <stdint.h>
#include <linux/types.h>
#include <linux/videodev.h>
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

#define   VIDEO_NODE_ANY              -1
#define   VIDEO_MODE_320x240_YUV422   32
#define   VIDEO_MODE_640x480_YUV411   33
#define   VIDEO_MODE_640x480_RGB      34
#define   VIDEO_MODE_640x480_MONO     35
#define   VIDEO_FRAME_RATE_1_875       1
#define   VIDEO_FRAME_RATE_3_75        2
#define   VIDEO_FRAME_RATE_7_5         3
#define   VIDEO_FRAME_RATE_15          4
#define   VIDEO_FRAME_RATE_30          5
#define   VIDEO_FRAME_RATE_60          6
#define   DEFAULT_VIDEO_NODE          VIDEO_NODE_ANY
#define   DEFAULT_VIDEO_MODE          VIDEO_MODE_640x480_YUV411
#define   DEFAULT_VIDEO_FRAME_RATE    VIDEO_FRAME_RATE_30

typedef struct {
	int      node;
	int      card;
	int      mode;
	int      rate;
	int      debug;

	int      channel;
	int      speed;
	int      format;
	int      dma_buf_num;
	int      int_mode;
	int      int_rate;
	int      status;

	int                    internal_id;
	dc1394_feature_set     features;
	dc1394_cameracapture   camera;
	uint8_t                *image;
} VideoParams;

int videoDisplayOptions();

int videoOpen (char *config);
VideoParams	*videoOpenHandle (char *config);

int videoClose();
int videoClose(VideoParams *vid);

int videoStartCapture();
int videoStartCapture(VideoParams *vid);

int videoStopCapture();
int videoStopCapture(VideoParams *vid);

int videoCaptureNext();
int videoCaptureNext(VideoParams *vid);

uint8_t *videoGetImage();
uint8_t *videoGetImage(VideoParams *vid);

int videoGetSize(int *x,int *y);
int videoGetSize(int *x,int *y,VideoParams *vid);

#endif
